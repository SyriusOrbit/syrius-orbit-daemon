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

        std::string payload;
        try {
            payload = nlohmann::json(value).dump();
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
    MqttPublisher& publisher_;
};

}  // namespace syrius_orbit
