#pragma once

#include <string>

#include "syrius_orbit/db/RobotsRepository.hpp"
#include "syrius_orbit/db/Vda5050EventsRepository.hpp"

namespace syrius_orbit {

/// Projects factsheet events into the robots read table.
class FactsheetProjector {
 public:
  explicit FactsheetProjector(RobotsRepository& robots_repo);

  /// Returns the VDA5050 topic this projector handles.
  static std::string topic() { return "factsheet"; }

  /// Parses the event payload and upserts the factsheet column.
  void project(const Vda5050Event& event);

 private:
  RobotsRepository& robots_repo_;
};

}  // namespace syrius_orbit