#pragma once

#include <algorithm>
#include <exception>
#include <functional>
#include <mosquittopp.h>
#include <mutex>
#include <plog/Log.h>
#include <string>
#include <utility>
#include <vector>

namespace syrius_orbit {

class MqttSubscriber : public mosqpp::mosquittopp {
public:
  using MessageCallback = std::function<void(const mosquitto_message &)>;

  explicit MqttSubscriber(const char *client_id = nullptr,
                          bool clean_session = true)
      : mosquittopp(client_id, clean_session) {}

  explicit MqttSubscriber(const std::string &client_id,
                          bool clean_session = true)
      : mosquittopp(client_id.c_str(), clean_session) {}

  [[nodiscard]] bool Subscribe(const std::string &topic, int qos,
                               MessageCallback callback) {
    if (topic.empty() || !callback)
      return false;

    const int rc = subscribe(nullptr, topic.c_str(), qos);
    if (rc != MOSQ_ERR_SUCCESS)
      return false;

    std::lock_guard lock(subscriptions_mutex_);
    subscriptions_.push_back(SubscriptionEntry{topic, std::move(callback)});

    return true;
  }

private:
  struct SubscriptionEntry {
    std::string topic_filter;
    MessageCallback callback;
  };

  void on_message(const mosquitto_message *msg) override {
    handle_message(msg);
  }

  void handle_message(const ::mosquitto_message *msg) const {
    if (msg == nullptr || msg->topic == nullptr)
      return;

    constexpr int kPayloadPreviewLimit = 120;
    const int payload_prefix_len =
        (msg->payload != nullptr && msg->payloadlen > 0)
            ? std::min(msg->payloadlen, kPayloadPreviewLimit)
            : 0;
    std::string payload_prefix;
    if (payload_prefix_len > 0)
      payload_prefix.assign(static_cast<const char *>(msg->payload),
                            static_cast<std::size_t>(payload_prefix_len));

    PLOGD << "MqttSubscriber received message: topic='" << msg->topic
          << "', qos=" << msg->qos << ", retain=" << msg->retain
          << ", mid=" << msg->mid << ", payload='" << payload_prefix
          << ((msg->payloadlen > payload_prefix_len) ? "...'." : "'.");

    std::vector<MessageCallback> callbacks;
    {
      std::lock_guard lock(subscriptions_mutex_);
      callbacks.reserve(subscriptions_.size());
      for (const auto &subscription : subscriptions_) {
        bool matched = false;
        const int match_rc = mosqpp::topic_matches_sub(
            subscription.topic_filter.c_str(), msg->topic, &matched);
        if (match_rc == MOSQ_ERR_SUCCESS && matched)
          callbacks.push_back(subscription.callback);
      }
    }

    for (const auto &callback : callbacks) {
      try {
        callback(*msg);
      } catch (const std::exception &ex) {
        PLOGE << "MqttSubscriber callback exception: " << ex.what();
      } catch (...) {
        PLOGE << "MqttSubscriber callback threw unknown exception.";
      }
    }
  }

  mutable std::mutex subscriptions_mutex_;
  std::vector<SubscriptionEntry> subscriptions_;
};

} // namespace syrius_orbit
