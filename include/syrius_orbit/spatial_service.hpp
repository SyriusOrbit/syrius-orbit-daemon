#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

#include "syrius_orbit/runtime_config.hpp"

namespace syrius_orbit {

class SpatialService {
public:
    bool init(const RuntimeConfig& config);
    bool start();
    void stop();
    [[nodiscard]] bool isRunning() const;

private:
    void run_server();
    void notify_start_result(bool success);

    RuntimeConfig config_;
    std::atomic_bool initialized_{false};
    std::atomic_bool running_{false};
    std::atomic_bool stop_requested_{false};
    std::thread worker_thread_;
    std::mutex start_mutex_;
    std::condition_variable start_cv_;
    std::mutex server_control_mutex_;
    std::function<void()> stop_server_fn_;
    bool start_done_{false};
    bool start_success_{false};
};

}  // namespace syrius_orbit
