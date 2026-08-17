#include "syrius_orbit/Daemon.hpp"
#include "syrius_orbit/FileLineFormatter.h"
#include "syrius_orbit/RuntimeConfig.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <fstream>
#include <thread>

#include <argparse/argparse.hpp>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>

namespace {

std::atomic_bool g_signal_received{false};

void on_signal(int) {
  g_signal_received.store(true, std::memory_order_relaxed);
}

bool load_json_config_file(const std::string &file_path,
                           syrius_orbit::RuntimeConfig &config_json) {
  if (file_path.empty()) {
    PLOGE << "Config file path is empty.";
    return false;
  }

  std::ifstream input(file_path);
  if (!input.good())
    PLOGE << "Can not find config file: " + file_path;
  if (!input.is_open()) {
    PLOGE << "Failed to open config file: " + file_path;
    return false;
  }

  try {
    std::string content((std::istreambuf_iterator(input)),
                        std::istreambuf_iterator<char>());
    nlohmann::json j = nlohmann::json::parse(content);
    config_json = j;
  } catch (const nlohmann::json::exception &e) {
    PLOGE << "Failed to parse JSON config file '" + file_path +
                 "': " + std::string(e.what())
          << "': " + std::string(e.what());
    return false;
  }

  return true;
}

} // namespace

int main(int argc, char **argv) {
  std::signal(SIGINT, &on_signal);
  std::signal(SIGTERM, &on_signal);

  argparse::ArgumentParser parser("syrius-orbit-daemon");
  syrius_orbit::RuntimeConfig config_cli;
  parser.add_argument("--config")
      .default_value(config_cli.config_file_path)
      .nargs(1)
      .metavar("path");
  parser.add_argument("--db-path")
      .default_value(config_cli.db_path)
      .nargs(1)
      .metavar("path");
  parser.add_argument("--http-host")
      .default_value(config_cli.http_host)
      .nargs(1)
      .metavar("host");
  parser.add_argument("--http-port")
      .default_value(std::to_string(config_cli.http_port))
      .nargs(1)
      .metavar("port");
  parser.add_argument("--mqtt-host")
      .default_value(config_cli.mqtt_host)
      .nargs(1)
      .metavar("host");
  parser.add_argument("--mqtt-port")
      .default_value(std::to_string(config_cli.mqtt_port))
      .nargs(1)
      .metavar("port");
  parser.add_argument("--mqtt-client-id")
      .default_value(config_cli.mqtt_client_id)
      .nargs(1)
      .metavar("id");
  parser.add_argument("--mqtt-username")
      .default_value(config_cli.mqtt_username)
      .nargs(1)
      .metavar("username");
  parser.add_argument("--mqtt-password")
      .default_value(config_cli.mqtt_password)
      .nargs(1)
      .metavar("password");
  parser.add_argument("--cloud-mqtt-host")
      .default_value(config_cli.cloud_mqtt_host)
      .nargs(1)
      .metavar("host");
  parser.add_argument("--cloud-mqtt-port")
      .default_value(std::to_string(config_cli.cloud_mqtt_port))
      .nargs(1)
      .metavar("port");
  parser.add_argument("--cloud-mqtt-client-id")
      .default_value(config_cli.cloud_mqtt_client_id)
      .nargs(1)
      .metavar("id");
  parser.add_argument("--cloud-mqtt-username")
      .default_value(config_cli.cloud_mqtt_username)
      .nargs(1)
      .metavar("username");
  parser.add_argument("--cloud-mqtt-password")
      .default_value(config_cli.cloud_mqtt_password)
      .nargs(1)
      .metavar("password");
  parser.add_argument("--mqtt-topic-prefix")
      .default_value(config_cli.mqtt_topic_prefix)
      .nargs(1)
      .metavar("prefix");
  parser.add_argument("-v", "--verbose")
      .help("print debug log")
      .default_value(false)
      .implicit_value(true);

  try {
    parser.parse_args(argc, argv);
  } catch (const std::exception &e) {
    LOGE << std::string(e.what()) + parser.usage();
    return EXIT_FAILURE;
  }

  config_cli.config_file_path = parser.get<std::string>("--config");
  config_cli.db_path = parser.get<std::string>("--db-path");
  config_cli.http_host = parser.get<std::string>("--http-host");
  config_cli.http_port = static_cast<std::uint16_t>(
      std::stoi(parser.get<std::string>("--http-port")));
  config_cli.mqtt_host = parser.get<std::string>("--mqtt-host");
  config_cli.mqtt_port = static_cast<std::uint16_t>(
      std::stoi(parser.get<std::string>("--mqtt-port")));
  config_cli.mqtt_client_id = parser.get<std::string>("--mqtt-client-id");
  config_cli.mqtt_username = parser.get<std::string>("--mqtt-username");
  config_cli.mqtt_password = parser.get<std::string>("--mqtt-password");
  config_cli.cloud_mqtt_host = parser.get<std::string>("--cloud-mqtt-host");
  config_cli.cloud_mqtt_port = static_cast<std::uint16_t>(
      std::stoi(parser.get<std::string>("--cloud-mqtt-port")));
  config_cli.cloud_mqtt_client_id =
      parser.get<std::string>("--cloud-mqtt-client-id");
  config_cli.cloud_mqtt_username =
      parser.get<std::string>("--cloud-mqtt-username");
  config_cli.cloud_mqtt_password =
      parser.get<std::string>("--cloud-mqtt-password");
  config_cli.mqtt_topic_prefix = parser.get<std::string>("--mqtt-topic-prefix");

  syrius_orbit::RuntimeConfig config_json;
  syrius_orbit::RuntimeConfig config = config_cli;
  if (not config_cli.config_file_path.empty()) {
    if (load_json_config_file(config_cli.config_file_path, config_json))
      config = config_json.Override(config_cli);
    else
      PLOGE << "Failed to load config file: " << config_cli.config_file_path;
  }

  static plog::ColorConsoleAppender<syrius_orbit::FileLineFormatter<true, true>>
      consoleAppender;
  static plog::RollingFileAppender<syrius_orbit::FileLineFormatter<true, true>>
      fileAppender("/var/log/syrius-orbit/daemon.log", 10000000, 5);

#ifdef NDEBUG
  if (parser.get<bool>("--verbose"))
    plog::init(plog::debug, &consoleAppender).addAppender(&fileAppender);
  else
    plog::init(plog::info, &consoleAppender).addAppender(&fileAppender);
#else
  plog::init(plog::debug, &consoleAppender);
#endif

  nlohmann::json j = config;
  PLOGD << j.dump(2);

  syrius_orbit::Daemon daemon(config);
  int daemon_exit_code = 1;
  std::atomic_bool daemon_finished{false};

  std::thread daemon_thread([&daemon, &daemon_exit_code, &daemon_finished] {
    daemon_exit_code = daemon.run();
    daemon_finished.store(true, std::memory_order_relaxed);
  });

  while (!g_signal_received.load(std::memory_order_relaxed) &&
         !daemon_finished.load(std::memory_order_relaxed)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  if (!daemon_finished.load(std::memory_order_relaxed))
    daemon.stop();
  daemon_thread.join();

  PLOGI << "Daemon exited with code: " << daemon_exit_code;

  return daemon_exit_code;
}
