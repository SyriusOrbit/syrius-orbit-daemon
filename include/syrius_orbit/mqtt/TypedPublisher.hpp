#pragma once

#include "syrius_orbit/mqtt/MqttPublisher.hpp"

#include <exception>
#include <nlohmann/json.hpp>
#include <plog/Log.h>
#include <string>

namespace syrius_orbit {

class TypedPublisher {
public:
    explicit TypedPublisher(MqttPublisher& publisher) noexcept : publisher_(publisher) {}

    template <typename T>
    [[nodiscard]] bool Publish(const std::string& topic, const T& value, int qos, bool retain) const {
        if (topic.empty()) {
            PLOGE << "TypedPublisher topic is empty.";
            return false;
        }

        nlohmann::ordered_json payload_json;
        std::string payload;
        try {
            payload_json = value;
            pruneTopLevelFields(payload_json);
            payload = payload_json.dump();
        } catch (const std::exception& ex) {
            PLOGE << "TypedPublisher failed to serialize payload for topic '" << topic
                  << "': " << ex.what();
            return false;
        } catch (...) {
            PLOGE << "TypedPublisher failed to serialize payload for topic '" << topic
                  << "' due to unknown exception.";
            return false;
        }

        const bool published = publisher_.Publish(topic, payload, qos, retain);
        if (!published) {
            PLOGE << "TypedPublisher failed to publish topic '" << topic << "'.";
            return false;
        }

        return true;
    }

private:
    static void pruneTopLevelFields(nlohmann::ordered_json& json) {
        if (!json.is_object())
            return;

        for (auto it = json.begin(); it != json.end();) {
            const bool remove_null = it.value().is_null();
            const bool remove_empty_array = it.value().is_array() && it.value().empty();
            if (remove_null || remove_empty_array) {
                it = json.erase(it);
                continue;
            }
            ++it;
        }
    }

    MqttPublisher& publisher_;
};

}  // namespace syrius_orbit
