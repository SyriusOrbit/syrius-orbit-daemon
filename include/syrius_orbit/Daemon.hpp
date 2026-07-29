#pragma once

#include <atomic>
#include <functional>
#include <mutex>

#include "syrius_orbit/FleetGateway.hpp"
#include "syrius_orbit/RuntimeConfig.hpp"
#include "syrius_orbit/SpatialService.hpp"

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
    std::mutex http_server_control_mutex_;
    std::function<void()> stop_http_server_fn_;
};

}  // namespace syrius_orbit
