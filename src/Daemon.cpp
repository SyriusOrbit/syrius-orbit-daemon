#include "syrius_orbit/Daemon.hpp"

#include <chrono>
#include <string>
#include <thread>
#include <utility>

#include <httplib.h>
#include <plog/Log.h>

#include "syrius_orbit/Vda5050EventRecorder.hpp"
#include "syrius_orbit/db/Database.hpp"

namespace syrius_orbit {

namespace {

void set_not_found_response(httplib::Response& res) {
    res.status = 404;
    res.body = "Not Found";
    res.set_header("Connection", "close");
}

}  // namespace

Daemon::Daemon(RuntimeConfig config)
    : config_(std::move(config)),
      db_(config_.db_path),
      events_repo_(db_),
      robots_repo_(db_),
      orders_repo_(db_),
      instant_actions_repo_(db_),
      maps_repo_(db_),
      projection_engine_(db_, robots_repo_, orders_repo_,
                         instant_actions_repo_,
                         config_.projection_interval_ms) {}

int Daemon::run() {
    try {
        db_.migrate();
    } catch (const std::exception& e) {
        PLOGE << "Schema migration failed (db_path=" << config_.db_path
              << "): " << e.what();
        return 1;
    }

    if (!fleet_gateway_.init(config_)) {
        PLOGE << "FleetGateway init failed.";
        return 1;
    }
    if (!fleet_gateway_.start()) {
        PLOGE << "FleetGateway start failed. Daemon startup aborted.";
        return 1;
    }

    Vda5050EventRecorder event_recorder(
        fleet_gateway_.proxy().localEndpoint(),
        fleet_gateway_.proxy().cloudEndpoint(), events_repo_);
    event_recorder.attach();

    fleet_gateway_.proxy().setOnCloudConnected([&event_recorder]() {
        event_recorder.attachCloud();
    });

    int exit_code = 0;
    PLOGI << "Daemon started.";

    httplib::Server http_server;
    http_server.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        if (req.method == "GET" && (req.path == "/console" || req.path == "/console/"))
            res.set_redirect("/console/index.html", 301);
        return httplib::Server::HandlerResponse::Unhandled;
    });
    http_server.set_error_handler([](const httplib::Request&, httplib::Response& res) {
        if (res.status == 404) {
            set_not_found_response(res);
        }
    });

    if (!spatial_service_.init(config_)) {
        PLOGE << "SpatialService init failed. Continuing without Spatial API.";
    } else if (!spatial_service_.bindRoutes(http_server) &&
               !stop_requested_.load(std::memory_order_relaxed)) {
        PLOGE << "SpatialService unavailable. Continuing without Spatial API.";
    }

    projection_engine_.start();

    if (!http_server.set_mount_point("/console", "web")) {
        PLOGE << "Static file mount failed for '/console' -> 'web'.";
    }

    bool http_server_started = false;
    std::thread http_thread;
    const std::string bind_host = (config_.http_host == "*") ? "0.0.0.0" : config_.http_host;
    if (!http_server.bind_to_port(bind_host, config_.http_port)) {
        PLOGE << "HTTP server failed to bind on " << config_.http_host << ":" << config_.http_port;
    } else {
        {
            std::lock_guard lock(http_server_control_mutex_);
            stop_http_server_fn_ = [&http_server]() { http_server.stop(); };
        }
        if (!stop_requested_.load(std::memory_order_relaxed)) {
            http_server_started = true;
            PLOGI << "HTTP server listening on " << config_.http_host << ":" << config_.http_port;
            http_thread = std::thread([&http_server]() { http_server.listen_after_bind(); });
        }
    }

    if (!stop_requested_.load(std::memory_order_relaxed)) {
        PLOGI << "Waiting for stop request.";
    }
    while (!stop_requested_.load(std::memory_order_relaxed)) {
        if (!fleet_gateway_.isRunning()) {
            PLOGE << "FleetGateway stopped unexpectedly.";
            exit_code = 1;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::function<void()> stop_http_server_fn;
    {
        std::lock_guard lock(http_server_control_mutex_);
        stop_http_server_fn = stop_http_server_fn_;
    }
    if (stop_http_server_fn) {
        stop_http_server_fn();
    }
    if (http_server_started && http_thread.joinable()) {
        http_thread.join();
    }
    {
        std::lock_guard lock(http_server_control_mutex_);
        stop_http_server_fn_ = nullptr;
    }

    projection_engine_.stop();
    spatial_service_.stop();
    fleet_gateway_.stop();

    PLOGI << "Stop signal received. Daemon exiting.";
    return exit_code;
}

void Daemon::stop() {
    stop_requested_.store(true, std::memory_order_relaxed);
    std::function<void()> stop_http_server_fn;
    {
        std::lock_guard lock(http_server_control_mutex_);
        stop_http_server_fn = stop_http_server_fn_;
    }
    if (stop_http_server_fn) {
        stop_http_server_fn();
    }
}

}  // namespace syrius_orbit
