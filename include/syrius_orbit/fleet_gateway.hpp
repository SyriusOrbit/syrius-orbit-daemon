#pragma once

#include <atomic>
#include <string>
#include <thread>

#include "syrius_orbit/runtime_config.hpp"

struct mosquitto;
struct mosquitto_message;

namespace syrius_orbit {

class FleetGateway {
public:
    bool init(const RuntimeConfig& config);
    bool start();
    void stop();
    [[nodiscard]] bool isRunning() const;

private:
    static void on_connect(mosquitto* mosq, void* obj, int rc);
    static void on_disconnect(mosquitto* mosq, void* obj, int rc);
    static void on_message(mosquitto* mosq, void* obj, const ::mosquitto_message* msg);

    void handle_connect(int rc);
    void handle_disconnect(int rc);
    void handle_message(const ::mosquitto_message* msg) const;
    void publish_online_status() const;

    RuntimeConfig config_;
    std::string command_topic_;
    std::string status_topic_;
    std::atomic_bool initialized_{false};
    std::atomic_bool running_{false};
    std::atomic_bool stop_requested_{false};
    std::atomic_bool connected_{false};
    std::thread worker_thread_;
    mosquitto* mosq_{nullptr};
};

}  // namespace syrius_orbit
