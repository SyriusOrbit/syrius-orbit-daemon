#include "syrius_orbit/projection/ProjectionEngine.hpp"

#include <chrono>
#include <functional>

#include <plog/Log.h>

namespace syrius_orbit {

ProjectionEngine::ProjectionEngine(Database& db,
                                   RobotsRepository& robots_repo,
                                   OrdersRepository& orders_repo,
                                   InstantActionsRepository& actions_repo,
                                   int interval_ms)
    : db_(db),
      events_repo_(db_),
      interval_ms_(interval_ms),
      connection_projector_(robots_repo),
      state_projector_(robots_repo),
      factsheet_projector_(robots_repo),
      order_projector_(orders_repo),
      instant_actions_projector_(actions_repo),
      responses_projector_(actions_repo) {}

ProjectionEngine::~ProjectionEngine() {
  stop();
}

void ProjectionEngine::start() {
  if (running_.exchange(true, std::memory_order_relaxed))
    return;

  thread_ = std::thread(&ProjectionEngine::run, this);
  PLOGI << "ProjectionEngine started (interval=" << interval_ms_ << "ms).";
}

void ProjectionEngine::stop() {
  if (!running_.exchange(false, std::memory_order_relaxed))
    return;

  if (thread_.joinable())
    thread_.join();

  PLOGI << "ProjectionEngine stopped.";
}

void ProjectionEngine::run() {
  // Each entry: topic name and a function that projects one event.
  struct Entry {
    std::string topic;
    std::function<void(const Vda5050Event&)> project;
  };

  const std::vector<Entry> projectors = {
      {ConnectionProjector::topic(),
       [this](const Vda5050Event& e) { connection_projector_.project(e); }},
      {StateProjector::topic(),
       [this](const Vda5050Event& e) { state_projector_.project(e); }},
      {FactsheetProjector::topic(),
       [this](const Vda5050Event& e) { factsheet_projector_.project(e); }},
      {OrderProjector::topic(),
       [this](const Vda5050Event& e) { order_projector_.project(e); }},
      {InstantActionsProjector::topic(),
       [this](const Vda5050Event& e) {
         instant_actions_projector_.project(e);
       }},
      {ResponsesProjector::topic(),
       [this](const Vda5050Event& e) { responses_projector_.project(e); }},
  };

  while (running_.load(std::memory_order_relaxed)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms_));

    if (!running_.load(std::memory_order_relaxed))
      break;

    try {
      std::lock_guard<Database> lock(db_);

      for (const auto& entry : projectors) {
        auto events = events_repo_.findLatestByTopic(entry.topic);
        for (const auto& event : events) {
          try {
            entry.project(event);
          } catch (const std::exception& e) {
            PLOGE << "Projection failed for topic=" << entry.topic
                  << " item_id=" << event.item_id << ": " << e.what();
          }
        }
      }
    } catch (const std::exception& e) {
      PLOGE << "Projection cycle failed: " << e.what();
    }
  }
}

}  // namespace syrius_orbit