#pragma once

#include <atomic>
#include <functional>
#include <mutex>

#include "syrius_orbit/RuntimeConfig.hpp"

namespace syrius_orbit {

class SpatialService {
public:
    bool init(const RuntimeConfig& config);
    bool start();
    void stop();
    [[nodiscard]] bool isRunning() const;

private:
    bool run_server();

    RuntimeConfig config_;
    std::atomic_bool initialized_{false};
    std::atomic_bool running_{false};
    std::atomic_bool stop_requested_{false};
    std::mutex server_control_mutex_;
    std::function<void()> stop_server_fn_;
};

}  // namespace syrius_orbit
