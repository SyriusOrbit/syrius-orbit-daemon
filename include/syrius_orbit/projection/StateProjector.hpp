#pragma once

#include <string>

#include "syrius_orbit/db/RobotsRepository.hpp"
#include "syrius_orbit/db/Vda5050EventsRepository.hpp"

namespace syrius_orbit {

/// Projects state events into the robots read table.
class StateProjector {
 public:
  explicit StateProjector(RobotsRepository& robots_repo);

  /// Returns the VDA5050 topic this projector handles.
  static std::string topic() { return "state"; }

  /// Parses the event payload and upserts state-related columns.
  void project(const Vda5050Event& event);

 private:
  RobotsRepository& robots_repo_;
};

}  // namespace syrius_orbit