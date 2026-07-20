#include "syrius_orbit/fleet_gateway.hpp"

#include <cstring>

#include <mosquitto.h>
#include <plog/Log.h>

namespace syrius_orbit {

bool FleetGateway::init(const RuntimeConfig& config) {
    config_ = config;
    command_topic_ = config_.mqtt_topic_prefix + "/fleet/commands/#";
    status_topic_ = config_.mqtt_topic_prefix + "/fleet/status";
    initialized_.store(true, std::memory_order_relaxed);
    return true;
}

bool FleetGateway::start() {
    if (!initialized_.load(std::memory_order_relaxed)) {
        PLOGE << "FleetGateway start requested before init.";
        return false;
    }
    if (running_.load(std::memory_order_relaxed)) {
        return true;
    }

    stop_requested_.store(false, std::memory_order_relaxed);
    connected_.store(false, std::memory_order_relaxed);

    const int init_rc = mosquitto_lib_init();
    if (init_rc != MOSQ_ERR_SUCCESS) {
        PLOGE << "FleetGateway mosquitto_lib_init failed: " << init_rc;
        return false;
    }

    mosq_ = mosquitto_new(config_.mqtt_client_id.c_str(), true, this);
    if (mosq_ == nullptr) {
        PLOGE << "FleetGateway failed to create mosquitto client.";
        mosquitto_lib_cleanup();
        return false;
    }

    mosquitto_connect_callback_set(mosq_, &FleetGateway::on_connect);
    mosquitto_disconnect_callback_set(mosq_, &FleetGateway::on_disconnect);
    mosquitto_message_callback_set(mosq_, &FleetGateway::on_message);
    mosquitto_reconnect_delay_set(mosq_, 1, 30, true);

    if (!config_.mqtt_username.empty()) {
        const char* password = config_.mqtt_password.empty() ? nullptr : config_.mqtt_password.c_str();
        const int auth_rc = mosquitto_username_pw_set(mosq_, config_.mqtt_username.c_str(), password);
        if (auth_rc != MOSQ_ERR_SUCCESS) {
            PLOGE << "FleetGateway failed to set MQTT auth: " << auth_rc;
            mosquitto_destroy(mosq_);
            mosq_ = nullptr;
            mosquitto_lib_cleanup();
            return false;
        }
    }

    const int connect_rc = mosquitto_connect(mosq_, config_.mqtt_host.c_str(), config_.mqtt_port, 60);
    if (connect_rc != MOSQ_ERR_SUCCESS) {
        PLOGE << "FleetGateway failed to connect to MQTT broker "
              << config_.mqtt_host << ":" << config_.mqtt_port << ", rc=" << connect_rc;
        mosquitto_destroy(mosq_);
        mosq_ = nullptr;
        mosquitto_lib_cleanup();
        return false;
    }

    running_.store(true, std::memory_order_relaxed);
    worker_thread_ = std::thread([this] {
        const int loop_rc = mosquitto_loop_forever(mosq_, -1, 1);
        if (!stop_requested_.load(std::memory_order_relaxed) && loop_rc != MOSQ_ERR_SUCCESS) {
            PLOGE << "FleetGateway MQTT loop exited unexpectedly, rc=" << loop_rc;
        }
        connected_.store(false, std::memory_order_relaxed);
        running_.store(false, std::memory_order_relaxed);
    });

    PLOGI << "FleetGateway started. MQTT broker " << config_.mqtt_host << ":" << config_.mqtt_port;
    return true;
}

void FleetGateway::stop() {
    stop_requested_.store(true, std::memory_order_relaxed);

    if (mosq_ != nullptr) {
        const int disconnect_rc = mosquitto_disconnect(mosq_);
        if (disconnect_rc != MOSQ_ERR_SUCCESS && disconnect_rc != MOSQ_ERR_NO_CONN) {
            PLOGW << "FleetGateway disconnect returned rc=" << disconnect_rc;
        }
    }

    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }

    if (mosq_ != nullptr) {
        mosquitto_destroy(mosq_);
        mosq_ = nullptr;
    }
    mosquitto_lib_cleanup();

    connected_.store(false, std::memory_order_relaxed);
    running_.store(false, std::memory_order_relaxed);
    PLOGI << "FleetGateway stopped.";
}

bool FleetGateway::isRunning() const {
    return running_.load(std::memory_order_relaxed);
}

void FleetGateway::on_connect(mosquitto* /*mosq*/, void* obj, int rc) {
    if (obj == nullptr) {
        return;
    }
    auto* self = static_cast<FleetGateway*>(obj);
    self->handle_connect(rc);
}

void FleetGateway::on_disconnect(mosquitto* /*mosq*/, void* obj, int rc) {
    if (obj == nullptr) {
        return;
    }
    auto* self = static_cast<FleetGateway*>(obj);
    self->handle_disconnect(rc);
}

void FleetGateway::on_message(mosquitto* /*mosq*/, void* obj, const ::mosquitto_message* msg) {
    if (obj == nullptr || msg == nullptr) {
        return;
    }
    auto* self = static_cast<FleetGateway*>(obj);
    self->handle_message(msg);
}

void FleetGateway::handle_connect(int rc) {
    if (rc != 0) {
        connected_.store(false, std::memory_order_relaxed);
        PLOGW << "FleetGateway MQTT connect callback with rc=" << rc;
        return;
    }

    connected_.store(true, std::memory_order_relaxed);
    const int sub_rc = mosquitto_subscribe(mosq_, nullptr, command_topic_.c_str(), 1);
    if (sub_rc != MOSQ_ERR_SUCCESS) {
        PLOGE << "FleetGateway failed to subscribe topic '" << command_topic_ << "', rc=" << sub_rc;
    } else {
        PLOGI << "FleetGateway subscribed to " << command_topic_;
    }

    publish_online_status();
}

void FleetGateway::handle_disconnect(int rc) {
    connected_.store(false, std::memory_order_relaxed);
    if (stop_requested_.load(std::memory_order_relaxed)) {
        return;
    }
    PLOGW << "FleetGateway disconnected from broker (rc=" << rc << "), retrying indefinitely.";
}

void FleetGateway::handle_message(const ::mosquitto_message* msg) const {
    const std::string topic = msg->topic != nullptr ? msg->topic : "";
    std::string payload;
    if (msg->payload != nullptr && msg->payloadlen > 0) {
        payload.assign(static_cast<const char*>(msg->payload), static_cast<std::size_t>(msg->payloadlen));
    }
    PLOGI << "FleetGateway message received. topic=" << topic << " payload=" << payload;
}

void FleetGateway::publish_online_status() const {
    const char* payload = "{\"status\":\"online\"}";
    const int publish_rc = mosquitto_publish(
        mosq_,
        nullptr,
        status_topic_.c_str(),
        static_cast<int>(std::strlen(payload)),
        payload,
        1,
        false);

    if (publish_rc != MOSQ_ERR_SUCCESS) {
        PLOGE << "FleetGateway failed to publish online status to " << status_topic_ << ", rc=" << publish_rc;
    } else {
        PLOGI << "FleetGateway published online status to " << status_topic_;
    }
}

}  // namespace syrius_orbit
