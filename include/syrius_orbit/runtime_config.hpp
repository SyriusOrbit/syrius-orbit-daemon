#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace syrius_orbit {

struct RuntimeConfig {
  std::string config_file_path{"syrius_orbit_daemon.json"};
  std::string http_host{"0.0.0.0"};
  std::uint16_t http_port{8080};
  std::string mqtt_host{"127.0.0.1"};
  std::uint16_t mqtt_port{1883};
  std::string mqtt_client_id{"syrius-orbit-daemon"};
  std::string mqtt_username{};
  std::string mqtt_password{};
  std::string mqtt_topic_prefix{"vda5050/v3"};

  [[nodiscard]] RuntimeConfig Override(const RuntimeConfig &other) const {
    const RuntimeConfig default_config;
    RuntimeConfig result = *this;
    if (other.config_file_path != default_config.config_file_path)
      result.config_file_path = other.config_file_path;
    if (other.http_host != default_config.http_host)
      result.http_host = other.http_host;
    if (other.http_port != default_config.http_port)
      result.http_port = other.http_port;
    if (other.mqtt_host != default_config.mqtt_host)
      result.mqtt_host = other.mqtt_host;
    if (other.mqtt_port != default_config.mqtt_port)
      result.mqtt_port = other.mqtt_port;
    if (other.mqtt_client_id != default_config.mqtt_client_id)
      result.mqtt_client_id = other.mqtt_client_id;
    if (other.mqtt_username != default_config.mqtt_username)
      result.mqtt_username = other.mqtt_username;
    if (other.mqtt_password != default_config.mqtt_password)
      result.mqtt_password = other.mqtt_password;
    if (other.mqtt_topic_prefix != default_config.mqtt_topic_prefix)
      result.mqtt_topic_prefix = other.mqtt_topic_prefix;
    return result;
  }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RuntimeConfig, config_file_path, http_host,
                                   http_port, mqtt_host, mqtt_port,
                                   mqtt_client_id, mqtt_username, mqtt_password,
                                   mqtt_topic_prefix);

} // namespace syrius_orbit
