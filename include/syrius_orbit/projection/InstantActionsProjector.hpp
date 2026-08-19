#pragma once

#include <string>

#include "syrius_orbit/db/InstantActionsRepository.hpp"
#include "syrius_orbit/db/Vda5050EventsRepository.hpp"

namespace syrius_orbit {

/// Projects instant_actions events into the instant_actions read table.
class InstantActionsProjector {
 public:
  explicit InstantActionsProjector(InstantActionsRepository& actions_repo);

  /// Returns the VDA5050 topic this projector handles.
  static std::string topic() { return "instant_actions"; }

  /// Parses the event payload and upserts each action in the vector.
  void project(const Vda5050Event& event);

 private:
  InstantActionsRepository& actions_repo_;
};

}  // namespace syrius_orbit