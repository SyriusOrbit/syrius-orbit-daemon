#include "syrius_orbit/Vda5050EventRecorder.hpp"

#include <string>

#include <nlohmann/json.hpp>

namespace syrius_orbit {

void Vda5050EventRecorder::insertEvent(Vda5050EventsRepository &repo,
                                       const std::string &topic,
                                       const std::string &item_id,
                                       const VDA5050TopicContext &context,
                                       const std::string &direction,
                                       std::int64_t header_id,
                                       const std::string &version,
                                       const std::string &timestamp,
                                       const std::string &payload) {
  Vda5050Event event;
  event.topic = topic;
  event.item_id = item_id;
  event.manufacturer = context.manufacturer;
  event.serial_number = context.serialNumber;
  event.robot_id = context.manufacturer + "." + context.serialNumber;
  event.direction = direction;
  event.header_id = header_id;
  event.header_version = version;
  event.header_timestamp = timestamp;
  event.payload = payload;
  repo.insert(event);
}

Vda5050EventRecorder::Vda5050EventRecorder(VDA5050EndPoint *local_ep,
                                           VDA5050EndPoint *cloud_ep,
                                           Vda5050EventsRepository &repo)
    : local_ep_(local_ep), cloud_ep_(cloud_ep), repo_(repo) {}

void Vda5050EventRecorder::attach() {
  (void)local_ep_->OnState([this](const vda5050::State &msg,
                                   const MessageMeta &,
                                   const VDA5050TopicContext &context) {
    insertEvent(repo_, "state",
                context.manufacturer + "." + context.serialNumber,
                context, "uplink", msg.headerId, msg.version,
                msg.timestamp, nlohmann::json(msg).dump());
  });
  (void)local_ep_->OnVisualization(
      [this](const vda5050::Visualization &msg, const MessageMeta &,
              const VDA5050TopicContext &context) {
        insertEvent(repo_, "visualization",
                    context.manufacturer + "." + context.serialNumber,
                    context, "uplink", msg.headerId, msg.version,
                    msg.timestamp, nlohmann::json(msg).dump());
      });
  (void)local_ep_->OnConnection(
      [this](const vda5050::Connection &msg, const MessageMeta &,
              const VDA5050TopicContext &context) {
        insertEvent(repo_, "connection",
                    context.manufacturer + "." + context.serialNumber,
                    context, "uplink", msg.headerId, msg.version,
                    msg.timestamp, nlohmann::json(msg).dump());
      });
  (void)local_ep_->OnFactSheet(
      [this](const vda5050::FactSheet &msg, const MessageMeta &,
              const VDA5050TopicContext &context) {
        insertEvent(repo_, "factsheet",
                    context.manufacturer + "." + context.serialNumber,
                    context, "uplink", msg.headerId, msg.version,
                    msg.timestamp, nlohmann::json(msg).dump());
      });

  if (cloud_ep_ != nullptr)
    attachCloud();
}

void Vda5050EventRecorder::attachCloud() {
  if (cloud_ep_ == nullptr || attached_cloud_)
    return;
  attached_cloud_ = true;

  (void)cloud_ep_->OnOrder([this](const vda5050::Order &msg,
                                   const MessageMeta &,
                                   const VDA5050TopicContext &context) {
    insertEvent(repo_, "order", msg.orderId, context, "downlink",
                msg.headerId, msg.version, msg.timestamp,
                nlohmann::json(msg).dump());
  });
  (void)cloud_ep_->OnInstantActions(
      [this](const vda5050::InstantActions &msg, const MessageMeta &,
              const VDA5050TopicContext &context) {
        const std::string payload = nlohmann::json(msg).dump();
        for (const auto &action : msg.actions) {
          insertEvent(repo_, "instantActions", action.actionId,
                      context, "downlink", msg.headerId, msg.version,
                      msg.timestamp, payload);
        }
      });
  (void)cloud_ep_->OnResponses(
      [this](const vda5050::Responses &msg, const MessageMeta &,
              const VDA5050TopicContext &context) {
        const std::string payload = nlohmann::json(msg).dump();
        for (const auto &response : msg.responses) {
          insertEvent(repo_, "responses", response.requestId,
                      context, "downlink", msg.headerId, msg.version,
                      msg.timestamp, payload);
        }
      });
  (void)cloud_ep_->OnZoneSet(
      [this](const vda5050::ZoneSetMessage &msg, const MessageMeta &,
              const VDA5050TopicContext &context) {
        insertEvent(repo_, "zoneSet", msg.zoneSet.zoneSetId, context,
                    "downlink", msg.headerId, msg.version, msg.timestamp,
                    nlohmann::json(msg).dump());
      });
}

}  // namespace syrius_orbit