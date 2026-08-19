#pragma once

#include <string>

#include "syrius_orbit/db/InstantActionsRepository.hpp"
#include "syrius_orbit/db/Vda5050EventsRepository.hpp"

namespace syrius_orbit {

/// Projects responses events into the instant_actions read table.
class ResponsesProjector {
 public:
  explicit ResponsesProjector(InstantActionsRepository& actions_repo);

  /// Returns the VDA5050 topic this projector handles.
  static std::string topic() { return "responses"; }

  /// Parses the event payload and upserts each response in the vector.
  void project(const Vda5050Event& event);

 private:
  InstantActionsRepository& actions_repo_;
};

}  // namespace syrius_orbit