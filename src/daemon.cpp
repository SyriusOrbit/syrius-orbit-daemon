#include "syrius_orbit/daemon.hpp"

#include <chrono>
#include <thread>

#include <plog/Log.h>

namespace syrius_orbit {

int Daemon::run() const {
    PLOGI << "Daemon started. Waiting for stop request.";

    while (!stop_requested_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    PLOGI << "Stop signal received. Daemon exiting.";
    return 0;
}

void Daemon::stop() {
    stop_requested_ = true;
}

}  // namespace syrius_orbit
