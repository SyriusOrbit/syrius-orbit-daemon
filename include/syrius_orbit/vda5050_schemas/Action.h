#pragma once

#include "Common.hpp"

#include <nlohmann/json.hpp>

namespace syrius_orbit::vda5050 {

struct Action {
  std::string actionType{}; // startPause, stopPause, shutdown ...
  std::string actionId{};   // Suggestion: Use UUIDs.
  std::optional<std::string> actionDescriptor{};
  BlockingType blockingType{};
  std::optional<std::map<std::string, nlohmann::json>> actionParameters{};
  std::optional<bool> retriable{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Action, actionType, actionId,
                                                actionDescriptor, blockingType,
                                                actionParameters, retriable);

} // namespace syrius_orbit::vda5050