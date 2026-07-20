#include "syrius_orbit/runtime_config.hpp"

#include <cctype>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <unordered_map>

namespace syrius_orbit {

namespace {

std::string trim(std::string_view value) {
    std::size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start])) != 0) {
        ++start;
    }

    std::size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])) != 0) {
        --end;
    }

    return std::string(value.substr(start, end - start));
}

bool parse_port(const std::string& value, std::uint16_t& out_port) {
    unsigned int parsed = 0;
    const auto* begin = value.data();
    const auto* end = value.data() + value.size();
    const auto [ptr, ec] = std::from_chars(begin, end, parsed);
    if (ec != std::errc() || ptr != end || parsed == 0 || parsed > 65535) {
        return false;
    }
    out_port = static_cast<std::uint16_t>(parsed);
    return true;
}

std::string usage() {
    return "Usage: syrius-orbit-daemon [options]\n"
           "Options:\n"
           "  --help\n"
           "  --config=<path>\n"
           "  --http-host=<host>\n"
           "  --http-port=<port>\n"
           "  --mqtt-host=<host>\n"
           "  --mqtt-port=<port>\n"
           "  --mqtt-client-id=<id>\n"
           "  --mqtt-username=<username>\n"
           "  --mqtt-password=<password>\n"
           "  --mqtt-topic-prefix=<prefix>\n";
}

bool parse_cli(int argc, char** argv, std::unordered_map<std::string, std::string>& cli_values, std::string& error) {
    for (int i = 1; i < argc; ++i) {
        const std::string token = argv[i];
        if (token == "--help") {
            error = usage();
            return false;
        }
        if (!token.starts_with("--")) {
            error = "Unsupported positional argument: " + token;
            return false;
        }

        std::string key;
        std::string value;
        const std::size_t eq_pos = token.find('=');
        if (eq_pos != std::string::npos) {
            key = token.substr(2, eq_pos - 2);
            value = token.substr(eq_pos + 1);
        } else {
            key = token.substr(2);
            if (i + 1 >= argc) {
                error = "Missing value for option --" + key;
                return false;
            }
            value = argv[++i];
        }

        if (key.empty()) {
            error = "Invalid empty option name.";
            return false;
        }
        cli_values[key] = value;
    }
    return true;
}

bool parse_kv_file(
    const std::string& file_path,
    bool required,
    std::unordered_map<std::string, std::string>& values,
    std::string& error) {
    std::error_code fs_error;
    const bool exists = std::filesystem::exists(file_path, fs_error);
    if (fs_error) {
        error = "Failed to check config file path '" + file_path + "': " + fs_error.message();
        return false;
    }
    if (!exists) {
        if (required) {
            error = "Config file does not exist: " + file_path;
            return false;
        }
        return true;
    }

    std::ifstream input(file_path);
    if (!input.is_open()) {
        error = "Failed to open config file: " + file_path;
        return false;
    }

    std::string line;
    std::size_t line_no = 0;
    while (std::getline(input, line)) {
        ++line_no;
        if (line_no == 1 && line.size() >= 3 &&
            static_cast<unsigned char>(line[0]) == 0xEF &&
            static_cast<unsigned char>(line[1]) == 0xBB &&
            static_cast<unsigned char>(line[2]) == 0xBF) {
            line.erase(0, 3);
        }

        const std::string stripped = trim(line);
        if (stripped.empty() || stripped.starts_with('#')) {
            continue;
        }

        const std::size_t eq_pos = stripped.find('=');
        if (eq_pos == std::string::npos) {
            error = "Invalid config line " + std::to_string(line_no) + " in " + file_path;
            return false;
        }

        std::string key = trim(stripped.substr(0, eq_pos));
        std::string value = trim(stripped.substr(eq_pos + 1));
        if (key.empty()) {
            error = "Empty key in config line " + std::to_string(line_no) + " in " + file_path;
            return false;
        }
        values[key] = value;
    }
    return true;
}

bool apply_value(RuntimeConfig& config, const std::string& key, const std::string& value, std::string& error) {
    if (key == "http-host" || key == "http_host") {
        config.http_host = value;
        return true;
    }
    if (key == "http-port" || key == "http_port") {
        std::uint16_t parsed_port = 0;
        if (!parse_port(value, parsed_port)) {
            error = "Invalid HTTP port: " + value;
            return false;
        }
        config.http_port = parsed_port;
        return true;
    }
    if (key == "mqtt-host" || key == "mqtt_host") {
        config.mqtt_host = value;
        return true;
    }
    if (key == "mqtt-port" || key == "mqtt_port") {
        std::uint16_t parsed_port = 0;
        if (!parse_port(value, parsed_port)) {
            error = "Invalid MQTT port: " + value;
            return false;
        }
        config.mqtt_port = parsed_port;
        return true;
    }
    if (key == "mqtt-client-id" || key == "mqtt_client_id") {
        config.mqtt_client_id = value;
        return true;
    }
    if (key == "mqtt-username" || key == "mqtt_username") {
        config.mqtt_username = value;
        return true;
    }
    if (key == "mqtt-password" || key == "mqtt_password") {
        config.mqtt_password = value;
        return true;
    }
    if (key == "mqtt-topic-prefix" || key == "mqtt_topic_prefix") {
        config.mqtt_topic_prefix = value;
        return true;
    }
    if (key == "config") {
        config.config_file_path = value;
        return true;
    }

    error = "Unsupported config key: " + key;
    return false;
}

bool validate(const RuntimeConfig& config, std::string& error) {
    if (config.http_host.empty()) {
        error = "HTTP host must not be empty.";
        return false;
    }
    if (config.mqtt_host.empty()) {
        error = "MQTT host must not be empty.";
        return false;
    }
    if (config.mqtt_client_id.empty()) {
        error = "MQTT client id must not be empty.";
        return false;
    }
    if (config.mqtt_topic_prefix.empty()) {
        error = "MQTT topic prefix must not be empty.";
        return false;
    }
    return true;
}

}  // namespace

RuntimeConfigLoadStatus RuntimeConfigLoader::load(
    int argc,
    char** argv,
    RuntimeConfig& out_config,
    std::string& message) {
    RuntimeConfig config;
    std::unordered_map<std::string, std::string> cli_values;
    message.clear();

    if (!parse_cli(argc, argv, cli_values, message)) {
        if (message == usage()) {
            return RuntimeConfigLoadStatus::kHelpRequested;
        }
        return RuntimeConfigLoadStatus::kError;
    }

    const auto config_it = cli_values.find("config");
    const bool has_cli_config = config_it != cli_values.end();
    const std::string config_file = has_cli_config ? config_it->second : config.config_file_path;
    config.config_file_path = config_file;

    std::unordered_map<std::string, std::string> file_values;
    if (!parse_kv_file(config_file, has_cli_config, file_values, message)) {
        return RuntimeConfigLoadStatus::kError;
    }

    for (const auto& [key, value] : file_values) {
        if (!apply_value(config, key, value, message)) {
            return RuntimeConfigLoadStatus::kError;
        }
    }
    for (const auto& [key, value] : cli_values) {
        if (!apply_value(config, key, value, message)) {
            return RuntimeConfigLoadStatus::kError;
        }
    }

    if (!validate(config, message)) {
        return RuntimeConfigLoadStatus::kError;
    }

    out_config = config;
    return RuntimeConfigLoadStatus::kOk;
}

}  // namespace syrius_orbit
