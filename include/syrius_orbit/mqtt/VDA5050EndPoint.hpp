#pragma once

#include "syrius_orbit/mqtt/VDA5050Publisher.hpp"
#include "syrius_orbit/mqtt/VDA5050Subscriber.hpp"
#include "syrius_orbit/mqtt/VDA5050TopicContext.hpp"
#include "syrius_orbit/vda5050_schemas/Connection.hpp"
#include "syrius_orbit/vda5050_schemas/FactSheet.hpp"
#include "syrius_orbit/vda5050_schemas/InstantActions.hpp"
#include "syrius_orbit/vda5050_schemas/Order.hpp"
#include "syrius_orbit/vda5050_schemas/Responses.hpp"
#include "syrius_orbit/vda5050_schemas/State.hpp"
#include "syrius_orbit/vda5050_schemas/Visualization.hpp"
#include "syrius_orbit/vda5050_schemas/ZoneSet.hpp"

#include <utility>

namespace syrius_orbit {

class VDA5050EndPoint {
public:
    enum class Side {
        Local,
        Cloud,
    };

    VDA5050EndPoint(MqttSubscriber& mqtt_client, Side side, VDA5050TopicContext topic_template)
        : mqtt_client_(mqtt_client),
          mqtt_publisher_(mqtt_client_),
          typed_publisher_(mqtt_publisher_),
          typed_subscriber_(mqtt_client_),
          publisher_(typed_publisher_),
          subscriber_(typed_subscriber_, std::move(topic_template)),
          side_(side) {}

    [[nodiscard]] bool Init() {
        if (!subscriber_.Init())
            return false;

        if (side_ == Side::Local)
            return subscriber_.SubscribeState() &&
                   subscriber_.SubscribeVisualization() &&
                   subscriber_.SubscribeConnection() &&
                   subscriber_.SubscribeFactSheet();

        return subscriber_.SubscribeOrder() &&
               subscriber_.SubscribeInstantActions() &&
               subscriber_.SubscribeResponses() &&
               subscriber_.SubscribeZoneSet();
    }

    void Stop() {
        subscriber_.Stop();
        publisher_.Stop();
    }

    [[nodiscard]] bool OnState(VDA5050Subscriber::StateCallback callback) {
        if (side_ != Side::Local)
            return false;
        return subscriber_.OnState(std::move(callback));
    }

    [[nodiscard]] bool OnVisualization(VDA5050Subscriber::VisualizationCallback callback) {
        if (side_ != Side::Local)
            return false;
        return subscriber_.OnVisualization(std::move(callback));
    }

    [[nodiscard]] bool OnConnection(VDA5050Subscriber::ConnectionCallback callback) {
        if (side_ != Side::Local)
            return false;
        return subscriber_.OnConnection(std::move(callback));
    }

    [[nodiscard]] bool OnFactSheet(VDA5050Subscriber::FactSheetCallback callback) {
        if (side_ != Side::Local)
            return false;
        return subscriber_.OnFactSheet(std::move(callback));
    }

    [[nodiscard]] bool OnOrder(VDA5050Subscriber::OrderCallback callback) {
        if (side_ != Side::Cloud)
            return false;
        return subscriber_.OnOrder(std::move(callback));
    }

    [[nodiscard]] bool OnInstantActions(VDA5050Subscriber::InstantActionsCallback callback) {
        if (side_ != Side::Cloud)
            return false;
        return subscriber_.OnInstantActions(std::move(callback));
    }

    [[nodiscard]] bool OnResponses(VDA5050Subscriber::ResponsesCallback callback) {
        if (side_ != Side::Cloud)
            return false;
        return subscriber_.OnResponses(std::move(callback));
    }

    [[nodiscard]] bool OnZoneSet(VDA5050Subscriber::ZoneSetCallback callback) {
        if (side_ != Side::Cloud)
            return false;
        return subscriber_.OnZoneSet(std::move(callback));
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context, const vda5050::Order& message) {
        if (side_ != Side::Local)
            return false;
        return publisher_.Publish(context, message);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context, const vda5050::InstantActions& message) {
        if (side_ != Side::Local)
            return false;
        return publisher_.Publish(context, message);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context, const vda5050::Responses& message) {
        if (side_ != Side::Local)
            return false;
        return publisher_.Publish(context, message);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context, const vda5050::ZoneSetMessage& message) {
        if (side_ != Side::Local)
            return false;
        return publisher_.Publish(context, message);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context, const vda5050::State& message) {
        if (side_ != Side::Cloud)
            return false;
        return publisher_.Publish(context, message);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context, const vda5050::Visualization& message) {
        if (side_ != Side::Cloud)
            return false;
        return publisher_.Publish(context, message);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context, const vda5050::Connection& message) {
        if (side_ != Side::Cloud)
            return false;
        return publisher_.Publish(context, message);
    }

    [[nodiscard]] bool Publish(const VDA5050TopicContext& context, const vda5050::FactSheet& message) {
        if (side_ != Side::Cloud)
            return false;
        return publisher_.Publish(context, message);
    }

private:
    MqttSubscriber& mqtt_client_;
    MqttPublisher mqtt_publisher_;
    TypedPublisher typed_publisher_;
    TypedSubscriber typed_subscriber_;
    VDA5050Publisher publisher_;
    VDA5050Subscriber subscriber_;
    Side side_;
};

}  // namespace syrius_orbit
