#pragma once

#include <mosquittopp.h>
#include <string>

namespace syrius_orbit {

class MqttPublisher {
public:
  explicit MqttPublisher(mosqpp::mosquittopp &mosq) noexcept
      : mosq_(&mosq) {}

  [[nodiscard]] bool Publish(const std::string &topic,
                             const std::string &payload, int qos = 0,
                             bool retain = false) const {
    if (mosq_ == nullptr || topic.empty())
      return false;

    int mid = 0;
    const int rc = mosq_->publish(&mid, topic.c_str(),
                                  static_cast<int>(payload.size()),
                                  payload.data(), qos, retain);
    return rc == MOSQ_ERR_SUCCESS;
  }

private:
  mosqpp::mosquittopp *mosq_{nullptr};
};

} // namespace syrius_orbit
