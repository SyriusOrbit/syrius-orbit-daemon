#include "syrius_orbit/daemon.hpp"

#include <chrono>
#include <thread>
#include <utility>

#include <plog/Log.h>

namespace syrius_orbit {

Daemon::Daemon(RuntimeConfig config) : config_(std::move(config)) {}

int Daemon::run() {
    if (!fleet_gateway_.init(config_)) {
        PLOGE << "FleetGateway init failed.";
        return 1;
    }
    if (!fleet_gateway_.start()) {
        PLOGE << "FleetGateway start failed. Daemon startup aborted.";
        return 1;
    }

    if (!spatial_service_.init(config_)) {
        PLOGE << "SpatialService init failed. Continuing without Spatial API.";
    } else if (!spatial_service_.start()) {
        PLOGE << "SpatialService start failed. Continuing without Spatial API.";
    }

    PLOGI << "Daemon started. Waiting for stop request.";

    int exit_code = 0;
    while (!stop_requested_.load(std::memory_order_relaxed)) {
        if (!fleet_gateway_.isRunning()) {
            PLOGE << "FleetGateway stopped unexpectedly.";
            exit_code = 1;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    spatial_service_.stop();
    fleet_gateway_.stop();

    PLOGI << "Stop signal received. Daemon exiting.";
    return exit_code;
}

void Daemon::stop() {
    stop_requested_.store(true, std::memory_order_relaxed);
}

}  // namespace syrius_orbit
