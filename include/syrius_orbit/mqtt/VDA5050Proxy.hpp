#pragma once

#include "syrius_orbit/RuntimeConfig.hpp"
#include "MqttSubscriber.hpp"
#include "VDA5050EndPoint.hpp"
#include "VDA5050TopicContext.hpp"

#include <atomic>
#include <memory>
#include <string>

namespace syrius_orbit {

class Vda5050EventsRepository;

class VDA5050Proxy {
public:
  bool init(const RuntimeConfig &config);
  bool start();
  void stop();
  [[nodiscard]] bool isRunning() const;

  void setVda5050EventsRepository(Vda5050EventsRepository &repo);

private:
  static bool parse_topic_prefix(const std::string &prefix,
                                 VDA5050TopicContext &topic_context);
  bool configure_client(MqttSubscriber &client, const std::string &client_id,
                        const std::string &username,
                        const std::string &password) const;
  bool connect_client(MqttSubscriber &client, const std::string &host,
                      std::uint16_t port, const char *name) const;
  bool setup_bridge_callbacks();
  bool bridge_local_to_cloud();
  bool bridge_cloud_to_local();

  RuntimeConfig config_;
  std::atomic_bool initialized_{false};
  std::atomic_bool running_{false};
  std::atomic_bool mosquitto_initialized_{false};
  MqttSubscriber local_client_;
  MqttSubscriber cloud_client_;
  std::unique_ptr<VDA5050EndPoint> local_endpoint_;
  std::unique_ptr<VDA5050EndPoint> cloud_endpoint_;
  Vda5050EventsRepository *events_repo_{nullptr};
};

} // namespace syrius_orbit
