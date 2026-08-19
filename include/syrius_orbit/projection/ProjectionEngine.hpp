#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "syrius_orbit/db/Database.hpp"
#include "syrius_orbit/db/Vda5050EventsRepository.hpp"
#include "syrius_orbit/db/RobotsRepository.hpp"
#include "syrius_orbit/db/OrdersRepository.hpp"
#include "syrius_orbit/db/InstantActionsRepository.hpp"
#include "syrius_orbit/projection/ConnectionProjector.hpp"
#include "syrius_orbit/projection/StateProjector.hpp"
#include "syrius_orbit/projection/FactsheetProjector.hpp"
#include "syrius_orbit/projection/OrderProjector.hpp"
#include "syrius_orbit/projection/InstantActionsProjector.hpp"
#include "syrius_orbit/projection/ResponsesProjector.hpp"

namespace syrius_orbit {

/// Runs the CQRS projection loop: periodically queries the latest events
/// per entity from the write model and upserts them into the read model.
class ProjectionEngine {
 public:
  /// @param db           Database instance (shared mutex with HTTP server).
  /// @param robots_repo  Robots read repository.
  /// @param orders_repo  Orders read repository.
  /// @param actions_repo Instant actions read repository.
  /// @param interval_ms  Projection cycle interval in milliseconds.
  ProjectionEngine(Database& db,
                   RobotsRepository& robots_repo,
                   OrdersRepository& orders_repo,
                   InstantActionsRepository& actions_repo,
                   int interval_ms);

  ~ProjectionEngine();

  ProjectionEngine(const ProjectionEngine&) = delete;
  ProjectionEngine& operator=(const ProjectionEngine&) = delete;

  /// Starts the projection loop on a background thread.
  void start();

  /// Stops the projection loop and joins the background thread.
  void stop();

 private:
  void run();

  Database& db_;
  Vda5050EventsRepository events_repo_;
  int interval_ms_;

  ConnectionProjector connection_projector_;
  StateProjector state_projector_;
  FactsheetProjector factsheet_projector_;
  OrderProjector order_projector_;
  InstantActionsProjector instant_actions_projector_;
  ResponsesProjector responses_projector_;

  std::atomic_bool running_{false};
  std::thread thread_;
};

}  // namespace syrius_orbit