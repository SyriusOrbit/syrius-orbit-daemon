#pragma once

#include "syrius_orbit/mqtt/MqttSubscriber.hpp"

#include <exception>
#include <functional>
#include <nlohmann/json.hpp>
#include <plog/Log.h>
#include <string>
#include <utility>

namespace syrius_orbit {

struct MessageMeta {
    std::string topic;
    int qos{0};
    bool retain{false};
    std::string payload;
    int mid{0};
};

class TypedSubscriber {
public:
    template <typename T>
    using Callback = std::function<void(const T&, const MessageMeta&)>;

    explicit TypedSubscriber(MqttSubscriber& subscriber) noexcept : subscriber_(subscriber) {}

    template <typename T>
    [[nodiscard]] bool Subscribe(const std::string& topic_filter, int qos, Callback<T> callback) {
        if (topic_filter.empty() || !callback) {
            return false;
        }

        auto wrapped_callback = [callback = std::move(callback)](const ::mosquitto_message& msg) {
            if (msg.topic == nullptr) {
                PLOGW << "TypedSubscriber received message with null topic.";
                return;
            }
            if (msg.payloadlen < 0) {
                PLOGW << "TypedSubscriber received message with negative payload length.";
                return;
            }

            MessageMeta meta;
            meta.topic = msg.topic;
            meta.qos = msg.qos;
            meta.retain = msg.retain != 0;
            meta.mid = msg.mid;
            if (msg.payload != nullptr && msg.payloadlen > 0)
                meta.payload.assign(static_cast<const char*>(msg.payload), static_cast<std::size_t>(msg.payloadlen));

            T payload;
            try {
                const nlohmann::json payload_json = nlohmann::json::parse(meta.payload);
                payload = payload_json.get<T>();
            } catch (const std::exception& ex) {
                PLOGE << "TypedSubscriber failed to deserialize payload for topic '" << meta.topic
                      << "': " << ex.what();
                return;
            } catch (...) {
                PLOGE << "TypedSubscriber failed to deserialize payload for topic '" << meta.topic
                      << "' due to unknown exception.";
                return;
            }

            try {
                callback(payload, meta);
            } catch (const std::exception& ex) {
                PLOGE << "TypedSubscriber typed callback exception: " << ex.what();
            } catch (...) {
                PLOGE << "TypedSubscriber typed callback threw unknown exception.";
            }
        };

        return subscriber_.Subscribe(topic_filter, qos, std::move(wrapped_callback));
    }

private:
    MqttSubscriber& subscriber_;
};

}  // namespace syrius_orbit
