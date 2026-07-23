#pragma once

#include <exception>
#include <functional>
#include <mosquitto.h>
#include <mutex>
#include <plog/Log.h>
#include <string>
#include <utility>
#include <vector>

namespace syrius_orbit {

class MqttSubscriber {
public:
  using MessageCallback = std::function<void(const mosquitto_message &)>;

  explicit MqttSubscriber(mosquitto *mosq) noexcept : mosq_(mosq) {}

  [[nodiscard]] bool Subscribe(const std::string &topic, int qos,
                               MessageCallback callback) {
    if (mosq_ == nullptr || topic.empty() || !callback)
      return false;

    mosquitto_user_data_set(mosq_, this);
    mosquitto_message_callback_set(mosq_, &MqttSubscriber::on_message);

    const int rc = mosquitto_subscribe(mosq_, nullptr, topic.c_str(), qos);
    if (rc != MOSQ_ERR_SUCCESS)
      return false;
    {
      std::lock_guard lock(subscriptions_mutex_);
      subscriptions_.push_back(SubscriptionEntry{topic, std::move(callback)});
    }

    return true;
  }

private:
  struct SubscriptionEntry {
    std::string topic_filter;
    MessageCallback callback;
  };

  static void on_message(mosquitto * /*mosq*/, void *obj,
                         const ::mosquitto_message *msg) {
    if (obj == nullptr)
      return;

    auto *self = static_cast<MqttSubscriber *>(obj);
    self->handle_message(msg);
  }

  void handle_message(const ::mosquitto_message *msg) const {
    if (msg == nullptr || msg->topic == nullptr)
      return;

    std::vector<MessageCallback> callbacks;
    {
      std::lock_guard lock(subscriptions_mutex_);
      callbacks.reserve(subscriptions_.size());
      for (const auto &subscription : subscriptions_) {
        bool matched = false;
        const int match_rc = mosquitto_topic_matches_sub(
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

  mosquitto *mosq_{nullptr};
  mutable std::mutex subscriptions_mutex_;
  std::vector<SubscriptionEntry> subscriptions_;
};

} // namespace syrius_orbit
