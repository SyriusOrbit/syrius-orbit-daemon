#include "syrius_orbit/FleetGateway.hpp"

#include <plog/Log.h>

namespace syrius_orbit {

bool FleetGateway::init(const RuntimeConfig& config) {
    if (!proxy_.init(config)) {
        PLOGE << "FleetGateway failed to init VDA5050Proxy.";
        return false;
    }
    initialized_.store(true, std::memory_order_relaxed);
    return true;
}

bool FleetGateway::start() {
    if (!initialized_.load(std::memory_order_relaxed)) {
        PLOGE << "FleetGateway start requested before init.";
        return false;
    }
    if (proxy_.isRunning()) {
        return true;
    }
    if (!proxy_.start()) {
        PLOGE << "FleetGateway failed to start VDA5050Proxy.";
        return false;
    }
    PLOGI << "FleetGateway started.";
    return true;
}

void FleetGateway::stop() {
    proxy_.stop();
    PLOGI << "FleetGateway stopped.";
}

bool FleetGateway::isRunning() const {
    return proxy_.isRunning();
}

}  // namespace syrius_orbit
