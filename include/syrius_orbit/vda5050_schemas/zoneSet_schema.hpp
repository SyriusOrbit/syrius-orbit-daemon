#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050::schema::zoneSet {

struct Vertex {
  double x{};
  double y{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Vertex, x, y);

struct ActionParameter {
  std::string key{};
  nlohmann::json value{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ActionParameter, key, value);

struct ZoneAction {
  std::string actionType{};
  std::optional<std::string> actionDescriptor{};
  std::string blockingType{};
  std::optional<std::vector<ActionParameter>> actionParameters{};
  std::optional<bool> retriable{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ZoneAction, actionType, actionDescriptor, blockingType, actionParameters, retriable);

struct Zone {
  std::string zoneId{};
  std::string zoneType{};
  std::optional<std::string> zoneDescriptor{};
  std::vector<Vertex> vertices{};
  std::optional<std::string> releaseLossBehavior{};
  std::optional<double> maximumSpeed{};
  std::optional<std::vector<ZoneAction>> entryActions{};
  std::optional<std::vector<ZoneAction>> duringActions{};
  std::optional<std::vector<ZoneAction>> exitActions{};
  std::optional<double> priorityFactor{};
  std::optional<double> penaltyFactor{};
  std::optional<double> direction{};
  std::optional<std::string> directedLimitation{};
  std::optional<std::string> bidirectedLimitation{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Zone, zoneId, zoneType, zoneDescriptor, vertices, releaseLossBehavior, maximumSpeed, entryActions, duringActions, exitActions, priorityFactor, penaltyFactor, direction, directedLimitation, bidirectedLimitation);

struct ZoneSet {
  std::string mapId{};
  std::string zoneSetId{};
  std::optional<std::string> zoneSetDescriptor{};
  std::vector<Zone> zones{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ZoneSet, mapId, zoneSetId, zoneSetDescriptor, zones);

struct ZoneSetMessage {
  std::int64_t headerId{};
  std::string timestamp{};
  std::string version{};
  std::string manufacturer{};
  std::string serialNumber{};
  ZoneSet zoneSet{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ZoneSetMessage, headerId, timestamp, version, manufacturer, serialNumber, zoneSet);

} // namespace syrius_orbit::vda5050::schema::zoneSet
