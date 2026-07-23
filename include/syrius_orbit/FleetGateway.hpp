#pragma once

#include <atomic>

#include "syrius_orbit/RuntimeConfig.hpp"
#include "syrius_orbit/mqtt/VDA5050Proxy.hpp"

namespace syrius_orbit {

class FleetGateway {
public:
    bool init(const RuntimeConfig& config);
    bool start();
    void stop();
    [[nodiscard]] bool isRunning() const;

private:
    std::atomic_bool initialized_{false};
    VDA5050Proxy proxy_;
};

}  // namespace syrius_orbit
