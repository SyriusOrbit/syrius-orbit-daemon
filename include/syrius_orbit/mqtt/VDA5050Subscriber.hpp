#pragma once

#include "VDA5050TopicContext.hpp"
#include "TypedSubscriber.hpp"
#include "syrius_orbit/vda5050_schemas/InstantActions.hpp"
#include "syrius_orbit/vda5050_schemas/Connection.hpp"
#include "syrius_orbit/vda5050_schemas/FactSheet.hpp"
#include "syrius_orbit/vda5050_schemas/Order.hpp"
#include "syrius_orbit/vda5050_schemas/Responses.hpp"
#include "syrius_orbit/vda5050_schemas/State.hpp"
#include "syrius_orbit/vda5050_schemas/Visualization.hpp"
#include "syrius_orbit/vda5050_schemas/ZoneSet.hpp"

#include <exception>
#include <functional>
#include <atomic>
#include <mutex>
#include <plog/Log.h>
#include <string>
#include <utility>
#include <vector>

namespace syrius_orbit {

class VDA5050Subscriber {
public:
    using StateCallback =
        std::function<void(const vda5050::State&, const MessageMeta&, const VDA5050TopicContext&)>;
    using VisualizationCallback =
        std::function<void(const vda5050::Visualization&, const MessageMeta&, const VDA5050TopicContext&)>;
    using ConnectionCallback =
        std::function<void(const vda5050::Connection&, const MessageMeta&, const VDA5050TopicContext&)>;
    using FactSheetCallback =
        std::function<void(const vda5050::FactSheet&, const MessageMeta&, const VDA5050TopicContext&)>;
    using OrderCallback =
        std::function<void(const vda5050::Order&, const MessageMeta&, const VDA5050TopicContext&)>;
    using InstantActionsCallback =
        std::function<void(const vda5050::InstantActions&, const MessageMeta&, const VDA5050TopicContext&)>;
    using ResponsesCallback =
        std::function<void(const vda5050::Responses&, const MessageMeta&, const VDA5050TopicContext&)>;
    using ZoneSetCallback =
        std::function<void(const vda5050::ZoneSetMessage&, const MessageMeta&, const VDA5050TopicContext&)>;

    VDA5050Subscriber(TypedSubscriber& typed_subscriber, VDA5050TopicContext topic_template)
        : typed_subscriber_(typed_subscriber), topic_template_(std::move(topic_template)) {}

    [[nodiscard]] bool Init() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (stopped_.load(std::memory_order_relaxed)) {
            PLOGW << "VDA5050Subscriber init requested after stop.";
            return false;
        }
        if (initialized_)
            return true;
        if (topic_template_.interfaceName.empty() || topic_template_.majorVersion.empty())
            return false;

        initialized_ = true;
        return true;
    }

    [[nodiscard]] bool OnState(StateCallback callback) {
        if (!callback)
            return false;
        if (stopped_.load(std::memory_order_relaxed)) {
            PLOGW << "VDA5050Subscriber state callback requested after stop.";
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        state_callbacks_.push_back(std::move(callback));
        return true;
    }

    [[nodiscard]] bool OnVisualization(VisualizationCallback callback) {
        if (!callback)
            return false;
        if (stopped_.load(std::memory_order_relaxed)) {
            PLOGW << "VDA5050Subscriber visualization callback requested after stop.";
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        visualization_callbacks_.push_back(std::move(callback));
        return true;
    }

    [[nodiscard]] bool OnConnection(ConnectionCallback callback) {
        if (!callback)
            return false;
        if (stopped_.load(std::memory_order_relaxed)) {
            PLOGW << "VDA5050Subscriber connection callback requested after stop.";
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        connection_callbacks_.push_back(std::move(callback));
        return true;
    }

    [[nodiscard]] bool OnFactSheet(FactSheetCallback callback) {
        if (!callback)
            return false;
        if (stopped_.load(std::memory_order_relaxed)) {
            PLOGW << "VDA5050Subscriber factsheet callback requested after stop.";
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        factsheet_callbacks_.push_back(std::move(callback));
        return true;
    }

    [[nodiscard]] bool OnOrder(OrderCallback callback) {
        if (!callback)
            return false;
        if (stopped_.load(std::memory_order_relaxed)) {
            PLOGW << "VDA5050Subscriber order callback requested after stop.";
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        order_callbacks_.push_back(std::move(callback));
        return true;
    }

    [[nodiscard]] bool OnInstantActions(InstantActionsCallback callback) {
        if (!callback)
            return false;
        if (stopped_.load(std::memory_order_relaxed)) {
            PLOGW << "VDA5050Subscriber instantActions callback requested after stop.";
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        instant_actions_callbacks_.push_back(std::move(callback));
        return true;
    }

    [[nodiscard]] bool OnResponses(ResponsesCallback callback) {
        if (!callback)
            return false;
        if (stopped_.load(std::memory_order_relaxed)) {
            PLOGW << "VDA5050Subscriber responses callback requested after stop.";
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        responses_callbacks_.push_back(std::move(callback));
        return true;
    }

    [[nodiscard]] bool OnZoneSet(ZoneSetCallback callback) {
        if (!callback)
            return false;
        if (stopped_.load(std::memory_order_relaxed)) {
            PLOGW << "VDA5050Subscriber zoneSet callback requested after stop.";
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        zone_set_callbacks_.push_back(std::move(callback));
        return true;
    }

    [[nodiscard]] bool SubscribeState() {
        return subscribe<vda5050::State>("state", 0,
            [this](const vda5050::State& msg, const MessageMeta& meta) { handleState(msg, meta); });
    }

    [[nodiscard]] bool SubscribeVisualization() {
        return subscribe<vda5050::Visualization>("visualization", 0,
            [this](const vda5050::Visualization& msg, const MessageMeta& meta) {
                handleVisualization(msg, meta);
            });
    }

    [[nodiscard]] bool SubscribeConnection() {
        return subscribe<vda5050::Connection>("connection", 1,
            [this](const vda5050::Connection& msg, const MessageMeta& meta) { handleConnection(msg, meta); });
    }

    [[nodiscard]] bool SubscribeFactSheet() {
        return subscribe<vda5050::FactSheet>("factsheet", 0,
            [this](const vda5050::FactSheet& msg, const MessageMeta& meta) { handleFactSheet(msg, meta); });
    }

    [[nodiscard]] bool SubscribeOrder() {
        return subscribe<vda5050::Order>("order", 0,
            [this](const vda5050::Order& msg, const MessageMeta& meta) { handleOrder(msg, meta); });
    }

    [[nodiscard]] bool SubscribeInstantActions() {
        return subscribe<vda5050::InstantActions>("instantActions", 0,
            [this](const vda5050::InstantActions& msg, const MessageMeta& meta) {
                handleInstantActions(msg, meta);
            });
    }

    [[nodiscard]] bool SubscribeResponses() {
        return subscribe<vda5050::Responses>("responses", 0,
            [this](const vda5050::Responses& msg, const MessageMeta& meta) { handleResponses(msg, meta); });
    }

    [[nodiscard]] bool SubscribeZoneSet() {
        return subscribe<vda5050::ZoneSetMessage>("zoneSet", 0,
            [this](const vda5050::ZoneSetMessage& msg, const MessageMeta& meta) { handleZoneSet(msg, meta); });
    }

    void Stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_.store(true, std::memory_order_relaxed);
        state_callbacks_.clear();
        visualization_callbacks_.clear();
        connection_callbacks_.clear();
        factsheet_callbacks_.clear();
        order_callbacks_.clear();
        instant_actions_callbacks_.clear();
        responses_callbacks_.clear();
        zone_set_callbacks_.clear();
    }

private:
    [[nodiscard]] std::string buildFilter(const std::string& suffix) const {
        return topic_template_.interfaceName + "/" + topic_template_.majorVersion + "/+/+/" + suffix;
    }

    [[nodiscard]] bool parseTopicContext(const std::string& topic,
                                         const std::string& suffix,
                                         VDA5050TopicContext& context) const {
        const std::string expected_prefix = topic_template_.interfaceName + "/" + topic_template_.majorVersion + "/";
        if (topic.rfind(expected_prefix, 0) != 0)
            return false;

        const std::string remainder = topic.substr(expected_prefix.size());
        const std::size_t first_sep = remainder.find('/');
        if (first_sep == std::string::npos || first_sep == 0)
            return false;
        const std::size_t second_sep = remainder.find('/', first_sep + 1U);
        if (second_sep == std::string::npos || second_sep == first_sep + 1U)
            return false;

        const std::string tail = remainder.substr(second_sep + 1U);
        if (tail != suffix)
            return false;

        context.interfaceName = topic_template_.interfaceName;
        context.majorVersion = topic_template_.majorVersion;
        context.manufacturer = remainder.substr(0, first_sep);
        context.serialNumber = remainder.substr(first_sep + 1U, second_sep - first_sep - 1U);
        return !context.manufacturer.empty() && !context.serialNumber.empty();
    }

    template <typename TMessage, typename TCallback>
    [[nodiscard]] bool subscribe(const std::string& suffix,
                                 int qos,
                                 TCallback callback) {
        if (stopped_.load(std::memory_order_relaxed)) {
            PLOGW << "VDA5050Subscriber subscribe requested after stop.";
            return false;
        }
        std::string filter = buildFilter(suffix);
        if (filter.empty())
            return false;
        if (!typed_subscriber_.Subscribe<TMessage>(filter, qos, std::move(callback)))
            return false;
        return true;
    }

    template <typename TMessage, typename TCallback>
    void dispatch(const TMessage& message,
                  const MessageMeta& meta,
                  const std::string& suffix,
                  const std::vector<TCallback>& callbacks,
                  const char* type_name) const {
        if (stopped_.load(std::memory_order_relaxed))
            return;
        VDA5050TopicContext context;
        if (!parseTopicContext(meta.topic, suffix, context))
            return;

        for (const auto& callback : callbacks) {
            try {
                callback(message, meta, context);
            } catch (const std::exception& ex) {
                PLOGE << "VDA5050Subscriber " << type_name << " callback exception: " << ex.what();
            } catch (...) {
                PLOGE << "VDA5050Subscriber " << type_name << " callback unknown exception.";
            }
        }
    }

    void handleState(const vda5050::State& message, const MessageMeta& meta) {
        std::vector<StateCallback> callbacks;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            callbacks = state_callbacks_;
        }
        dispatch(message, meta, "state", callbacks, "state");
    }

    void handleVisualization(const vda5050::Visualization& message, const MessageMeta& meta) {
        std::vector<VisualizationCallback> callbacks;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            callbacks = visualization_callbacks_;
        }
        dispatch(message, meta, "visualization", callbacks, "visualization");
    }

    void handleConnection(const vda5050::Connection& message, const MessageMeta& meta) {
        std::vector<ConnectionCallback> callbacks;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            callbacks = connection_callbacks_;
        }
        dispatch(message, meta, "connection", callbacks, "connection");
    }

    void handleFactSheet(const vda5050::FactSheet& message, const MessageMeta& meta) {
        std::vector<FactSheetCallback> callbacks;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            callbacks = factsheet_callbacks_;
        }
        dispatch(message, meta, "factsheet", callbacks, "factsheet");
    }

    void handleOrder(const vda5050::Order& message, const MessageMeta& meta) {
        std::vector<OrderCallback> callbacks;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            callbacks = order_callbacks_;
        }
        dispatch(message, meta, "order", callbacks, "order");
    }

    void handleInstantActions(const vda5050::InstantActions& message, const MessageMeta& meta) {
        std::vector<InstantActionsCallback> callbacks;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            callbacks = instant_actions_callbacks_;
        }
        dispatch(message, meta, "instantActions", callbacks, "instantActions");
    }

    void handleResponses(const vda5050::Responses& message, const MessageMeta& meta) {
        std::vector<ResponsesCallback> callbacks;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            callbacks = responses_callbacks_;
        }
        dispatch(message, meta, "responses", callbacks, "responses");
    }

    void handleZoneSet(const vda5050::ZoneSetMessage& message, const MessageMeta& meta) {
        std::vector<ZoneSetCallback> callbacks;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            callbacks = zone_set_callbacks_;
        }
        dispatch(message, meta, "zoneSet", callbacks, "zoneSet");
    }

    TypedSubscriber& typed_subscriber_;
    VDA5050TopicContext topic_template_;

    mutable std::mutex mutex_;
    bool initialized_{false};
    std::atomic_bool stopped_{false};
    std::vector<StateCallback> state_callbacks_;
    std::vector<VisualizationCallback> visualization_callbacks_;
    std::vector<ConnectionCallback> connection_callbacks_;
    std::vector<FactSheetCallback> factsheet_callbacks_;
    std::vector<OrderCallback> order_callbacks_;
    std::vector<InstantActionsCallback> instant_actions_callbacks_;
    std::vector<ResponsesCallback> responses_callbacks_;
    std::vector<ZoneSetCallback> zone_set_callbacks_;
};

}  // namespace syrius_orbit
