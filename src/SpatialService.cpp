#include "syrius_orbit/SpatialService.hpp"

#include <httplib.h>
#include <plog/Log.h>

namespace syrius_orbit {

namespace {
void set_not_found_response(httplib::Response& res) {
    res.status = 404;
    res.body = "Not Found";
    res.set_header("Connection", "close");
}

}  // namespace

bool SpatialService::init(const RuntimeConfig& config) {
    config_ = config;
    initialized_.store(true, std::memory_order_relaxed);
    return true;
}

bool SpatialService::bindRoutes(httplib::Server& server) {
    if (!initialized_.load(std::memory_order_relaxed)) {
        PLOGE << "SpatialService route binding requested before init.";
        return false;
    }

    if (running_.load(std::memory_order_relaxed)) {
        return true;
    }

    server.Get("/health", [](const httplib::Request& req, httplib::Response& res) {
        if (req.target != "/health") {
            set_not_found_response(res);
            return;
        }

        res.status = 200;
        PLOGI << "SpatialService health check OK.";
        res.set_content(R"({"status":"ok"})", "application/json");
        res.set_header("Connection", "close");
    });

    running_.store(true, std::memory_order_relaxed);
    PLOGI << "SpatialService routes registered.";
    return true;
}

void SpatialService::stop() { running_.store(false, std::memory_order_relaxed); }

bool SpatialService::isRunning() const { return running_.load(std::memory_order_relaxed); }

}  // namespace syrius_orbit
