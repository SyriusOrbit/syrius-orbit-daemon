#pragma once

#include <atomic>
#include <functional>
#include <mutex>

#include "syrius_orbit/FleetGateway.hpp"
#include "syrius_orbit/RuntimeConfig.hpp"
#include "syrius_orbit/SpatialService.hpp"
#include "syrius_orbit/db/Database.hpp"
#include "syrius_orbit/db/Vda5050EventsRepository.hpp"
#include "syrius_orbit/db/RobotsRepository.hpp"
#include "syrius_orbit/db/OrdersRepository.hpp"
#include "syrius_orbit/db/InstantActionsRepository.hpp"
#include "syrius_orbit/db/MapsRepository.hpp"
#include "syrius_orbit/projection/ProjectionEngine.hpp"

namespace syrius_orbit {

class Daemon {
public:
    explicit Daemon(RuntimeConfig config);
    [[nodiscard]] int run();
    void stop();

private:
    RuntimeConfig config_;
    Database db_;
    Vda5050EventsRepository vda5050_events_repo_;
    RobotsRepository robots_repo_;
    OrdersRepository orders_repo_;
    InstantActionsRepository instant_actions_repo_;
    MapsRepository maps_repo_;
    SpatialService spatial_service_;
    FleetGateway fleet_gateway_;
    ProjectionEngine projection_engine_;
    std::atomic_bool stop_requested_{false};
    std::mutex http_server_control_mutex_;
    std::function<void()> stop_http_server_fn_;
};

}  // namespace syrius_orbit
