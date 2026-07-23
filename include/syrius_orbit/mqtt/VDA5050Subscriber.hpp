#pragma once

#include "syrius_orbit/mqtt/TypedSubscriber.hpp"
#include "syrius_orbit/vda5050_schemas/Connection.hpp"
#include "syrius_orbit/vda5050_schemas/FactSheet.hpp"
#include "syrius_orbit/vda5050_schemas/State.hpp"
#include "syrius_orbit/vda5050_schemas/Visualization.hpp"

#include <exception>
#include <functional>
#include <mutex>
#include <plog/Log.h>
#include <string>
#include <utility>
#include <vector>

namespace syrius_orbit {

struct VDA5050TopicContext {
    std::string interfaceName;
    std::string majorVersion;
    std::string manufacturer;
    std::string serialNumber;
};

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

    VDA5050Subscriber(TypedSubscriber& typed_subscriber, std::string interface_name, std::string major_version)
        : typed_subscriber_(typed_subscriber),
          interface_name_(std::move(interface_name)),
          major_version_(std::move(major_version)) {}

    [[nodiscard]] bool Init() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (initialized_) {
            return true;
        }
        if (interface_name_.empty() || major_version_.empty()) {
            PLOGE << "VDA5050Subscriber requires non-empty interface_name and major_version.";
            return false;
        }

        if (!typed_subscriber_.Subscribe<vda5050::State>(
                buildFilter("state"), 0, [this](const vda5050::State& msg, const MessageMeta& meta) {
                    handleState(msg, meta);
                })) {
            PLOGE << "VDA5050Subscriber failed to subscribe state.";
            return false;
        }

        if (!typed_subscriber_.Subscribe<vda5050::Visualization>(
                buildFilter("visualization"), 0, [this](const vda5050::Visualization& msg, const MessageMeta& meta) {
                    handleVisualization(msg, meta);
                })) {
            PLOGE << "VDA5050Subscriber failed to subscribe visualization.";
            return false;
        }

        if (!typed_subscriber_.Subscribe<vda5050::Connection>(
                buildFilter("connection"), 1, [this](const vda5050::Connection& msg, const MessageMeta& meta) {
                    handleConnection(msg, meta);
                })) {
            PLOGE << "VDA5050Subscriber failed to subscribe connection.";
            return false;
        }

        if (!typed_subscriber_.Subscribe<vda5050::FactSheet>(
                buildFilter("factsheet"), 0, [this](const vda5050::FactSheet& msg, const MessageMeta& meta) {
                    handleFactSheet(msg, meta);
                })) {
            PLOGE << "VDA5050Subscriber failed to subscribe factsheet.";
            return false;
        }

        initialized_ = true;
        return true;
    }

    [[nodiscard]] bool OnState(StateCallback callback) {
        if (!callback) {
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        state_callbacks_.push_back(std::move(callback));
        return true;
    }

    [[nodiscard]] bool OnVisualization(VisualizationCallback callback) {
        if (!callback) {
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        visualization_callbacks_.push_back(std::move(callback));
        return true;
    }

    [[nodiscard]] bool OnConnection(ConnectionCallback callback) {
        if (!callback) {
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        connection_callbacks_.push_back(std::move(callback));
        return true;
    }

    [[nodiscard]] bool OnFactSheet(FactSheetCallback callback) {
        if (!callback) {
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        factsheet_callbacks_.push_back(std::move(callback));
        return true;
    }

private:
    [[nodiscard]] std::string buildFilter(const std::string& suffix) const {
        return interface_name_ + "/" + major_version_ + "/+/+/" + suffix;
    }

    [[nodiscard]] bool parseTopicContext(const std::string& topic,
                                         const std::string& suffix,
                                         VDA5050TopicContext& context) const {
        const std::string expected_prefix = interface_name_ + "/" + major_version_ + "/";
        if (topic.rfind(expected_prefix, 0) != 0) {
            return false;
        }

        const std::string remainder = topic.substr(expected_prefix.size());
        const std::size_t first_sep = remainder.find('/');
        if (first_sep == std::string::npos || first_sep == 0) {
            return false;
        }
        const std::size_t second_sep = remainder.find('/', first_sep + 1U);
        if (second_sep == std::string::npos || second_sep == first_sep + 1U) {
            return false;
        }

        const std::string tail = remainder.substr(second_sep + 1U);
        if (tail != suffix) {
            return false;
        }

        context.interfaceName = interface_name_;
        context.majorVersion = major_version_;
        context.manufacturer = remainder.substr(0, first_sep);
        context.serialNumber = remainder.substr(first_sep + 1U, second_sep - first_sep - 1U);
        return !context.manufacturer.empty() && !context.serialNumber.empty();
    }

    template <typename TMessage, typename TCallback>
    void dispatch(const TMessage& message,
                  const MessageMeta& meta,
                  const std::string& suffix,
                  const std::vector<TCallback>& callbacks,
                  const char* type_name) const {
        VDA5050TopicContext context;
        if (!parseTopicContext(meta.topic, suffix, context)) {
            PLOGW << "VDA5050Subscriber " << type_name << " topic format invalid: " << meta.topic;
            return;
        }

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

    TypedSubscriber& typed_subscriber_;
    std::string interface_name_;
    std::string major_version_;

    mutable std::mutex mutex_;
    bool initialized_{false};
    std::vector<StateCallback> state_callbacks_;
    std::vector<VisualizationCallback> visualization_callbacks_;
    std::vector<ConnectionCallback> connection_callbacks_;
    std::vector<FactSheetCallback> factsheet_callbacks_;
};

}  // namespace syrius_orbit
