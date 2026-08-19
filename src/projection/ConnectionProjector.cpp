#include "syrius_orbit/projection/ConnectionProjector.hpp"

#include <plog/Log.h>

#include "syrius_orbit/vda5050_schemas/Connection.hpp"

namespace syrius_orbit {

ConnectionProjector::ConnectionProjector(RobotsRepository& robots_repo)
    : robots_repo_(robots_repo) {}

void ConnectionProjector::project(const Vda5050Event& event) {
  auto msg = nlohmann::json::parse(event.payload).get<vda5050::Connection>();
  std::string connection_state =
      nlohmann::json(msg.connectionState).get<std::string>();

  robots_repo_.upsertConnection(event.robot_id, event.manufacturer,
                                event.serial_number, connection_state,
                                event.received_at);
}

}  // namespace syrius_orbit