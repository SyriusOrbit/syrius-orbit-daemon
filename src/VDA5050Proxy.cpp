#include "syrius_orbit/mqtt/VDA5050Proxy.hpp"

#include <mosquittopp.h>
#include <plog/Log.h>

namespace syrius_orbit {

bool VDA5050Proxy::init(const RuntimeConfig &config) {
  config_ = config;
  initialized_.store(true, std::memory_order_relaxed);
  return true;
}

bool VDA5050Proxy::start() {
  if (!initialized_.load(std::memory_order_relaxed)) {
    PLOGE << "VDA5050Proxy start requested before init.";
    return false;
  }
  if (running_.load(std::memory_order_relaxed))
    return true;

  VDA5050TopicContext topic_template;
  if (!parse_topic_prefix(config_.mqtt_topic_prefix, topic_template)) {
    PLOGE << "Invalid mqtt_topic_prefix '" << config_.mqtt_topic_prefix
          << "'. Expected '<interfaceName>/<majorVersion>'.";
    return false;
  }

  const int init_rc = mosqpp::lib_init();
  if (init_rc != MOSQ_ERR_SUCCESS) {
    PLOGE << "VDA5050Proxy mosquitto lib init failed, rc=" << init_rc;
    return false;
  }
  mosquitto_initialized_.store(true, std::memory_order_relaxed);

  if (!configure_client(local_client_, config_.mqtt_client_id,
                        config_.mqtt_username, config_.mqtt_password) ||
      !configure_client(cloud_client_, config_.cloud_mqtt_client_id,
                        config_.cloud_mqtt_username,
                        config_.cloud_mqtt_password)) {
    stop();
    return false;
  }

  // Phase 1: local broker connection is required.
  if (!connect_client(local_client_, config_.mqtt_host, config_.mqtt_port,
                      "local")) {
    stop();
    return false;
  }

  local_endpoint_ = std::make_unique<VDA5050EndPoint>(
      local_client_, VDA5050EndPoint::Side::Local, topic_template);

  if (!local_endpoint_->Init()) {
    PLOGE << "VDA5050Proxy failed to initialize local endpoint.";
    stop();
    return false;
  }

  const int local_loop_rc = local_client_.loop_start();
  if (local_loop_rc != MOSQ_ERR_SUCCESS) {
    PLOGE << "VDA5050Proxy failed to start local MQTT loop, rc="
          << local_loop_rc;
    stop();
    return false;
  }

  // Phase 2: cloud broker connection is optional (async).
  cloud_client_.setConnectCallback([this](int rc) {
    if (rc == MOSQ_ERR_SUCCESS) {
      if (cloud_endpoint_ != nullptr)
        return;  // already set up (reconnect)

      VDA5050TopicContext topic_template_inner;
      if (!parse_topic_prefix(config_.mqtt_topic_prefix, topic_template_inner))
        return;

      cloud_endpoint_ = std::make_unique<VDA5050EndPoint>(
          cloud_client_, VDA5050EndPoint::Side::Cloud, topic_template_inner);

      if (!cloud_endpoint_->Init()) {
        PLOGE << "VDA5050Proxy failed to initialize cloud endpoint.";
        cloud_endpoint_.reset();
        return;
      }

      if (!setup_bridge_callbacks()) {
        PLOGE << "VDA5050Proxy failed to setup bridge callbacks.";
        cloud_endpoint_.reset();
        return;
      }

      PLOGI << "VDA5050Proxy cloud connected to " << config_.cloud_mqtt_host
            << ":" << config_.cloud_mqtt_port;

      if (on_cloud_connected_)
        on_cloud_connected_();
    } else {
      PLOGW << "VDA5050Proxy cloud connection failed (rc=" << rc
            << "), will retry.";
    }
  });

  const int cloud_loop_rc = cloud_client_.loop_start();
  if (cloud_loop_rc != MOSQ_ERR_SUCCESS) {
    PLOGE << "VDA5050Proxy failed to start cloud MQTT loop, rc="
          << cloud_loop_rc;
    // Local is already running; do not abort. Cloud loop is non-critical.
  }

  const int cloud_connect_rc = cloud_client_.connect_async(
      config_.cloud_mqtt_host.c_str(), static_cast<int>(config_.cloud_mqtt_port),
      60);
  if (cloud_connect_rc != MOSQ_ERR_SUCCESS) {
    PLOGW << "VDA5050Proxy cloud connect_async failed, rc=" << cloud_connect_rc
          << ". Proxy will start without cloud MQTT.";
  }

  running_.store(true, std::memory_order_relaxed);
  PLOGI << "VDA5050Proxy started. local=" << config_.mqtt_host << ":"
        << config_.mqtt_port << ", cloud=" << config_.cloud_mqtt_host << ":"
        << config_.cloud_mqtt_port;
  return true;
}

void VDA5050Proxy::stop() {
  if (local_endpoint_ != nullptr)
    local_endpoint_->Stop();
  if (cloud_endpoint_ != nullptr)
    cloud_endpoint_->Stop();

  local_client_.disconnect();
  cloud_client_.disconnect();
  local_client_.loop_stop(true);
  cloud_client_.loop_stop(true);

  local_endpoint_.reset();
  cloud_endpoint_.reset();

  if (mosquitto_initialized_.exchange(false, std::memory_order_relaxed)) {
    mosqpp::lib_cleanup();
  }
  running_.store(false, std::memory_order_relaxed);
}

bool VDA5050Proxy::isRunning() const {
  return running_.load(std::memory_order_relaxed);
}

bool VDA5050Proxy::parse_topic_prefix(const std::string &prefix,
                                      VDA5050TopicContext &topic_context) {
  if (prefix.empty())
    return false;
  const std::size_t sep = prefix.find('/');
  if (sep == std::string::npos || sep == 0 || sep == prefix.size() - 1U)
    return false;
  if (prefix.find('/', sep + 1U) != std::string::npos)
    return false;
  topic_context.interfaceName = prefix.substr(0, sep);
  topic_context.majorVersion = prefix.substr(sep + 1U);
  topic_context.manufacturer.clear();
  topic_context.serialNumber.clear();
  return !topic_context.interfaceName.empty() &&
         !topic_context.majorVersion.empty();
}

bool VDA5050Proxy::configure_client(MqttSubscriber &client,
                                    const std::string &client_id,
                                    const std::string &username,
                                    const std::string &password) const {
  const int reinit_rc = client.reinitialise(client_id.c_str(), true);
  if (reinit_rc != MOSQ_ERR_SUCCESS) {
    PLOGE << "VDA5050Proxy failed to initialize MQTT client '" << client_id
          << "', rc=" << reinit_rc;
    return false;
  }

  client.reconnect_delay_set(1, 30, true);

  const char *password_ptr = password.empty() ? nullptr : password.c_str();
  const int auth_rc = client.username_pw_set(
      username.empty() ? nullptr : username.c_str(), password_ptr);
  if (auth_rc != MOSQ_ERR_SUCCESS) {
    PLOGE << "VDA5050Proxy failed to configure MQTT auth for client '"
          << client_id << "', rc=" << auth_rc;
    return false;
  }
  return true;
}

bool VDA5050Proxy::connect_client(MqttSubscriber &client, const std::string &host,
                                  std::uint16_t port, const char *name) const {
  const int connect_rc = client.connect(host.c_str(), static_cast<int>(port), 60);
  if (connect_rc != MOSQ_ERR_SUCCESS) {
    PLOGE << "VDA5050Proxy failed to connect " << name << " MQTT broker "
          << host << ":" << port << ", rc=" << connect_rc;
    return false;
  }
  return true;
}

bool VDA5050Proxy::setup_bridge_callbacks() {
  return bridge_local_to_cloud() && bridge_cloud_to_local();
}

bool VDA5050Proxy::bridge_local_to_cloud() {
  if (local_endpoint_ == nullptr || cloud_endpoint_ == nullptr)
    return false;

  return local_endpoint_->OnState([this](const vda5050::State &msg,
                                         const MessageMeta &,
                                         const VDA5050TopicContext &context) {
           if (!cloud_endpoint_->Publish(context, msg))
             PLOGE << "VDA5050Proxy failed to forward state local->cloud.";
         }) &&
         local_endpoint_->OnVisualization(
             [this](const vda5050::Visualization &msg, const MessageMeta &,
                    const VDA5050TopicContext &context) {
               if (!cloud_endpoint_->Publish(context, msg))
                 PLOGE
                     << "VDA5050Proxy failed to forward visualization local->cloud.";
             }) &&
         local_endpoint_->OnConnection(
             [this](const vda5050::Connection &msg, const MessageMeta &,
                    const VDA5050TopicContext &context) {
               if (!cloud_endpoint_->Publish(context, msg))
                 PLOGE
                     << "VDA5050Proxy failed to forward connection local->cloud.";
             }) &&
         local_endpoint_->OnFactSheet([this](const vda5050::FactSheet &msg,
                                             const MessageMeta &,
                                             const VDA5050TopicContext &context) {
           if (!cloud_endpoint_->Publish(context, msg))
             PLOGE << "VDA5050Proxy failed to forward factsheet local->cloud.";
         });
}

bool VDA5050Proxy::bridge_cloud_to_local() {
  if (local_endpoint_ == nullptr || cloud_endpoint_ == nullptr)
    return false;

  return cloud_endpoint_->OnOrder([this](const vda5050::Order &msg,
                                         const MessageMeta &,
                                         const VDA5050TopicContext &context) {
           if (!local_endpoint_->Publish(context, msg))
             PLOGE << "VDA5050Proxy failed to forward order cloud->local.";
         }) &&
         cloud_endpoint_->OnInstantActions(
             [this](const vda5050::InstantActions &msg, const MessageMeta &,
                    const VDA5050TopicContext &context) {
               if (!local_endpoint_->Publish(context, msg))
                 PLOGE
                     << "VDA5050Proxy failed to forward instantActions cloud->local.";
             }) &&
         cloud_endpoint_->OnResponses(
             [this](const vda5050::Responses &msg, const MessageMeta &,
                    const VDA5050TopicContext &context) {
               if (!local_endpoint_->Publish(context, msg))
                 PLOGE << "VDA5050Proxy failed to forward responses cloud->local.";
             }) &&
         cloud_endpoint_->OnZoneSet([this](const vda5050::ZoneSetMessage &msg,
                                           const MessageMeta &,
                                           const VDA5050TopicContext &context) {
           if (!local_endpoint_->Publish(context, msg))
             PLOGE << "VDA5050Proxy failed to forward zoneSet cloud->local.";
         });
}

} // namespace syrius_orbit
