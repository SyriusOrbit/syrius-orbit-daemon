#pragma once

#include "syrius_orbit/mqtt/TypedPublisher.hpp"
#include "syrius_orbit/mqtt/VDA5050TopicContext.hpp"
#include "syrius_orbit/vda5050_schemas/Connection.hpp"
#include "syrius_orbit/vda5050_schemas/InstantActions.hpp"
#include "syrius_orbit/vda5050_schemas/FactSheet.hpp"
#include "syrius_orbit/vda5050_schemas/Order.hpp"
#include "syrius_orbit/vda5050_schemas/Responses.hpp"
#include "syrius_orbit/vda5050_schemas/State.hpp"
#include "syrius_orbit/vda5050_schemas/Visualization.hpp"
#include "syrius_orbit/vda5050_schemas/ZoneSet.hpp"

#include <atomic>
#include <plog/Log.h>
#include <string>

namespace syrius_orbit {

class VDA5050Publisher {
public:
    explicit VDA5050Publisher(TypedPublisher& typed_publisher) noexcept
        : typed_publisher_(typed_publisher) {}

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context,
                               const vda5050::Order& message) const {
        return publish(context, "order", message, 0, false);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context,
                               const vda5050::InstantActions& message) const {
        return publish(context, "instantActions", message, 0, false);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context,
                               const vda5050::Responses& message) const {
        return publish(context, "responses", message, 0, false);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context,
                               const vda5050::ZoneSetMessage& message) const {
        return publish(context, "zoneSet", message, 0, false);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context,
                               const vda5050::State& message) const {
        return publish(context, "state", message, 0, false);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context,
                               const vda5050::Visualization& message) const {
        return publish(context, "visualization", message, 0, false);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context,
                               const vda5050::Connection& message) const {
        return publish(context, "connection", message, 1, false);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context,
                               const vda5050::FactSheet& message) const {
        return publish(context, "factsheet", message, 0, false);
    }

    void Stop() {
        stopped_.store(true, std::memory_order_relaxed);
    }

private:
    [[nodiscard]] bool buildTopic(const VDA5050TopicContext& context,
                                  const std::string& suffix,
                                  std::string& topic) const {
        if (context.interfaceName.empty() || context.majorVersion.empty() ||
            context.manufacturer.empty() || context.serialNumber.empty() ||
            suffix.empty())
            return false;

        topic = context.interfaceName + "/" + context.majorVersion + "/" +
                context.manufacturer + "/" + context.serialNumber + "/" + suffix;
        return true;
    }

    template <typename TMessage>
    [[nodiscard]] bool publish(const VDA5050TopicContext& context,
                               const std::string& suffix,
                               const TMessage& message,
                               int qos,
                               bool retain) const {
        if (stopped_.load(std::memory_order_relaxed)) {
            PLOGW << "VDA5050Publisher publish requested after stop.";
            return false;
        }
        std::string topic;
        if (!buildTopic(context, suffix, topic))
            return false;
        return typed_publisher_.Publish(topic, message, qos, retain);
    }

    TypedPublisher& typed_publisher_;
    std::atomic_bool stopped_{false};
};

}  // namespace syrius_orbit
