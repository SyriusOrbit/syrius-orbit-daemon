#include "syrius_orbit/projection/ResponsesProjector.hpp"

#include <plog/Log.h>

#include "syrius_orbit/vda5050_schemas/Responses.hpp"

namespace syrius_orbit {

ResponsesProjector::ResponsesProjector(
    InstantActionsRepository& actions_repo)
    : actions_repo_(actions_repo) {}

void ResponsesProjector::project(const Vda5050Event& event) {
  auto msg =
      nlohmann::json::parse(event.payload).get<vda5050::Responses>();

  for (const auto& response : msg.responses) {
    std::string action_status =
        nlohmann::json(response.grantType).get<std::string>();

    actions_repo_.upsertFromResponse(response.requestId, event.robot_id,
                                     action_status, event.received_at);
  }
}

}  // namespace syrius_orbit