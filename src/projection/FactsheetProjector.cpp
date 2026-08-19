#include "syrius_orbit/projection/FactsheetProjector.hpp"

#include <plog/Log.h>

namespace syrius_orbit {

FactsheetProjector::FactsheetProjector(RobotsRepository& robots_repo)
    : robots_repo_(robots_repo) {}

void FactsheetProjector::project(const Vda5050Event& event) {
  robots_repo_.upsertFactsheet(event.robot_id, event.manufacturer,
                               event.serial_number, event.payload,
                               event.received_at);
}

}  // namespace syrius_orbit