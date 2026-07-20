#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050::schema::instantActions {

struct InstantActionsMessageActionsItemActionParametersItem {
  std::string key{};
  nlohmann::json value{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(InstantActionsMessageActionsItemActionParametersItem, key, value);

struct InstantActionsMessageActionsItem {
  std::string actionType{};
  std::string actionId{};
  std::optional<std::string> actionDescriptor{};
  std::string blockingType{};
  std::optional<std::vector<InstantActionsMessageActionsItemActionParametersItem>> actionParameters{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(InstantActionsMessageActionsItem, actionType, actionId, actionDescriptor, blockingType, actionParameters);

struct InstantActionsMessage {
  std::int64_t headerId{};
  std::string timestamp{};
  std::string version{};
  std::string manufacturer{};
  std::string serialNumber{};
  std::vector<InstantActionsMessageActionsItem> actions{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(InstantActionsMessage, headerId, timestamp, version, manufacturer, serialNumber, actions);

} // namespace syrius_orbit::vda5050::schema::instantActions
