#pragma once

#include "common.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050 {

enum class ZoneZoneType {
  BLOCKED,
  LINE_GUIDED,
  RELEASE,
  COORDINATED_REPLANNING,
  SPEED_LIMIT,
  ACTION,
  PRIORITY,
  PENALTY,
  DIRECTED,
  BIDIRECTED,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ZoneZoneType,
                             {
                                 {ZoneZoneType::BLOCKED, "BLOCKED"},
                                 {ZoneZoneType::LINE_GUIDED, "LINE_GUIDED"},
                                 {ZoneZoneType::RELEASE, "RELEASE"},
                                 {ZoneZoneType::COORDINATED_REPLANNING,
                                  "COORDINATED_REPLANNING"},
                                 {ZoneZoneType::SPEED_LIMIT, "SPEED_LIMIT"},
                                 {ZoneZoneType::ACTION, "ACTION"},
                                 {ZoneZoneType::PRIORITY, "PRIORITY"},
                                 {ZoneZoneType::PENALTY, "PENALTY"},
                                 {ZoneZoneType::DIRECTED, "DIRECTED"},
                                 {ZoneZoneType::BIDIRECTED, "BIDIRECTED"},
                             });

enum class ZoneReleaseLossBehavior {
  STOP,
  CONTINUE,
  EVACUATE,
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    ZoneReleaseLossBehavior,
    {
        {ZoneReleaseLossBehavior::STOP, "STOP"},
        {ZoneReleaseLossBehavior::CONTINUE, "CONTINUE"},
        {ZoneReleaseLossBehavior::EVACUATE, "EVACUATE"},
    });

enum class ZoneDirectedLimitation {
  SOFT,
  RESTRICTED,
  STRICT,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ZoneDirectedLimitation,
                             {
                                 {ZoneDirectedLimitation::SOFT, "SOFT"},
                                 {ZoneDirectedLimitation::RESTRICTED,
                                  "RESTRICTED"},
                                 {ZoneDirectedLimitation::STRICT, "STRICT"},
                             });

enum class ZoneBidirectedLimitation {
  SOFT,
  RESTRICTED,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ZoneBidirectedLimitation,
                             {
                                 {ZoneBidirectedLimitation::SOFT, "SOFT"},
                                 {ZoneBidirectedLimitation::RESTRICTED,
                                  "RESTRICTED"},
                             });

struct ZoneAction {
  std::string actionType{};
  std::optional<std::string> actionDescriptor{};
  BlockingType blockingType{};
  std::optional<std::map<std::string, nlohmann::json>> actionParameters{};
  std::optional<bool> retriable{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ZoneAction, actionType,
                                                actionDescriptor, blockingType,
                                                actionParameters, retriable);

struct Zone {
  std::string zoneId{};
  ZoneZoneType zoneType{};
  std::optional<std::string> zoneDescriptor{};
  std::vector<Vertex> vertices{};
  std::optional<ZoneReleaseLossBehavior> releaseLossBehavior{};
  std::optional<double> maximumSpeed{};
  std::optional<std::vector<ZoneAction>> entryActions{};
  std::optional<std::vector<ZoneAction>> duringActions{};
  std::optional<std::vector<ZoneAction>> exitActions{};
  std::optional<double> priorityFactor{};
  std::optional<double> penaltyFactor{};
  std::optional<double> direction{};
  std::optional<ZoneDirectedLimitation> directedLimitation{};
  std::optional<ZoneBidirectedLimitation> bidirectedLimitation{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    Zone, zoneId, zoneType, zoneDescriptor, vertices, releaseLossBehavior,
    maximumSpeed, entryActions, duringActions, exitActions, priorityFactor,
    penaltyFactor, direction, directedLimitation, bidirectedLimitation);

struct ZoneSetDefinition {
  std::string mapId{};
  std::string zoneSetId{};
  std::optional<std::string> zoneSetDescriptor{};
  std::vector<Zone> zones{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ZoneSetDefinition, mapId,
                                                zoneSetId, zoneSetDescriptor,
                                                zones);

struct ZoneSetMessage {
  std::int64_t headerId{};
  std::string timestamp{};
  std::string version{};
  std::string manufacturer{};
  std::string serialNumber{};
  ZoneSetDefinition zoneSet{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ZoneSetMessage, headerId,
                                                timestamp, version,
                                                manufacturer, serialNumber,
                                                zoneSet);

} // namespace syrius_orbit::vda5050
