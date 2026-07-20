#pragma once

#include <atomic>

#include "syrius_orbit/fleet_gateway.hpp"
#include "syrius_orbit/runtime_config.hpp"
#include "syrius_orbit/spatial_service.hpp"

namespace syrius_orbit {

class Daemon {
public:
    explicit Daemon(RuntimeConfig config);
    [[nodiscard]] int run();
    void stop();

private:
    RuntimeConfig config_;
    SpatialService spatial_service_;
    FleetGateway fleet_gateway_;
    std::atomic_bool stop_requested_{false};
};

}  // namespace syrius_orbit
