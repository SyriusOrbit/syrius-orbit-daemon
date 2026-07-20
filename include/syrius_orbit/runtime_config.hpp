#pragma once

#include <cstdint>
#include <string>

namespace syrius_orbit {

struct RuntimeConfig {
    std::string config_file_path{"syrius_orbit_daemon.conf"};
    std::string http_host{"0.0.0.0"};
    std::uint16_t http_port{8080};
    std::string mqtt_host{"127.0.0.1"};
    std::uint16_t mqtt_port{1883};
    std::string mqtt_client_id{"syrius-orbit-daemon"};
    std::string mqtt_username{};
    std::string mqtt_password{};
    std::string mqtt_topic_prefix{"vda5050/v3"};
};

enum class RuntimeConfigLoadStatus {
    kOk,
    kHelpRequested,
    kError
};

class RuntimeConfigLoader {
public:
    static RuntimeConfigLoadStatus load(int argc, char** argv, RuntimeConfig& out_config, std::string& message);
};

}  // namespace syrius_orbit
