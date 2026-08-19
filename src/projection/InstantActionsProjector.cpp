#include "syrius_orbit/projection/InstantActionsProjector.hpp"

#include <plog/Log.h>

#include "syrius_orbit/vda5050_schemas/InstantActions.hpp"

namespace syrius_orbit {

InstantActionsProjector::InstantActionsProjector(
    InstantActionsRepository& actions_repo)
    : actions_repo_(actions_repo) {}

void InstantActionsProjector::project(const Vda5050Event& event) {
  auto msg =
      nlohmann::json::parse(event.payload).get<vda5050::InstantActions>();

  for (const auto& action : msg.actions) {
    std::string blocking_type =
        nlohmann::json(action.blockingType).get<std::string>();
    std::string params_json =
        action.actionParameters.has_value()
            ? nlohmann::json(*action.actionParameters).dump()
            : "";

    actions_repo_.upsertFromAction(
        action.actionId, action.actionType,
        action.actionDescriptor.value_or(""), blocking_type, params_json,
        event.robot_id, event.received_at);
  }
}

}  // namespace syrius_orbit