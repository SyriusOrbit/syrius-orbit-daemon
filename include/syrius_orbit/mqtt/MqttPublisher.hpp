#pragma once

#include <mosquitto.h>
#include <string>

namespace syrius_orbit {

class MqttPublisher {
public:
  explicit MqttPublisher(mosquitto *mosq) noexcept : mosq_(mosq) {}

  [[nodiscard]] bool Publish(const std::string &topic,
                             const std::string &payload, int qos = 0,
                             bool retain = false) const {
    if (mosq_ == nullptr || topic.empty()) {
      return false;
    }

    const int rc = mosquitto_publish(mosq_, nullptr, topic.c_str(),
                                     static_cast<int>(payload.size()),
                                     payload.data(), qos, retain);
    return rc == MOSQ_ERR_SUCCESS;
  }

private:
  mosquitto *mosq_{nullptr};
};

} // namespace syrius_orbit
