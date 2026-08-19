#include "syrius_orbit/projection/StateProjector.hpp"

#include <plog/Log.h>

#include "syrius_orbit/vda5050_schemas/State.hpp"

namespace syrius_orbit {

StateProjector::StateProjector(RobotsRepository& robots_repo)
    : robots_repo_(robots_repo) {}

void StateProjector::project(const Vda5050Event& event) {
  auto msg = nlohmann::json::parse(event.payload).get<vda5050::State>();
  std::string operating_mode =
      nlohmann::json(msg.operatingMode).get<std::string>();

  robots_repo_.upsertState(
      event.robot_id, event.manufacturer, event.serial_number, operating_mode,
      msg.driving, msg.paused.value_or(false), msg.powerSupply.stateOfCharge,
      msg.powerSupply.charging, event.received_at);
}

}  // namespace syrius_orbit