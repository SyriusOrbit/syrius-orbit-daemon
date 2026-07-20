#include "syrius_orbit/spatial_service.hpp"

#include <string>

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

bool SpatialService::start() {
    if (!initialized_.load(std::memory_order_relaxed)) {
        PLOGE << "SpatialService start requested before init.";
        return false;
    }

    if (running_.load(std::memory_order_relaxed)) {
        return true;
    }

    stop_requested_.store(false, std::memory_order_relaxed);
    return run_server();
}

void SpatialService::stop() {
    stop_requested_.store(true, std::memory_order_relaxed);
    std::function<void()> stop_server_fn;
    {
        std::lock_guard lock(server_control_mutex_);
        stop_server_fn = stop_server_fn_;
    }
    if (stop_server_fn) {
        stop_server_fn();
    }
    running_.store(false, std::memory_order_relaxed);
}

bool SpatialService::isRunning() const {
    return running_.load(std::memory_order_relaxed);
}

bool SpatialService::run_server() {
    httplib::Server server;
    server.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        if (req.method == "HEAD" && req.path == "/health") {
            set_not_found_response(res);
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
    });
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
    server.set_error_handler([](const httplib::Request&, httplib::Response& res) {
        if (res.status == 404) {
            set_not_found_response(res);
        }
    });

    const std::string bind_host = (config_.http_host == "*") ? "0.0.0.0" : config_.http_host;
    if (!server.bind_to_port(bind_host, config_.http_port)) {
        PLOGE << "SpatialService failed to bind on " << config_.http_host << ":" << config_.http_port;
        return false;
    }

    {
        std::lock_guard lock(server_control_mutex_);
        stop_server_fn_ = [&server]() { server.stop(); };
    }

    if (stop_requested_.load(std::memory_order_relaxed)) {
        std::lock_guard lock(server_control_mutex_);
        stop_server_fn_ = nullptr;
        return true;
    }

    running_.store(true, std::memory_order_relaxed);
    PLOGI << "SpatialService listening on " << config_.http_host << ":" << config_.http_port;
    server.listen_after_bind();

    {
        std::lock_guard lock(server_control_mutex_);
        stop_server_fn_ = nullptr;
    }
    running_.store(false, std::memory_order_relaxed);
    if (stop_requested_.load(std::memory_order_relaxed)) {
        PLOGI << "SpatialService stopped.";
        return true;
    }

    PLOGE << "SpatialService stopped unexpectedly.";
    return false;
}

}  // namespace syrius_orbit
