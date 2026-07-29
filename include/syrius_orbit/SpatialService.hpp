#pragma once

#include <atomic>

#include "syrius_orbit/RuntimeConfig.hpp"

namespace httplib {
class Server;
}

namespace syrius_orbit {

class SpatialService {
public:
    bool init(const RuntimeConfig& config);
    bool bindRoutes(httplib::Server& server);
    void stop();
    [[nodiscard]] bool isRunning() const;

private:
    RuntimeConfig config_;
    std::atomic_bool initialized_{false};
    std::atomic_bool running_{false};
};

}  // namespace syrius_orbit
