#include "syrius_orbit/daemon.hpp"
#include "syrius_orbit/runtime_config.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Log.h>
#include <plog/Init.h>

namespace {
std::atomic_bool g_signal_received{false};
void on_signal(int) {
    g_signal_received.store(true, std::memory_order_relaxed);
}
}  // namespace

int main(int argc, char** argv) {
    static plog::ConsoleAppender<plog::TxtFormatter> console_appender;
    plog::init(plog::info, &console_appender);

    syrius_orbit::RuntimeConfig config;
    std::string config_message;
    const auto config_status = syrius_orbit::RuntimeConfigLoader::load(argc, argv, config, config_message);
    if (config_status == syrius_orbit::RuntimeConfigLoadStatus::kHelpRequested) {
        std::cout << config_message;
        return 0;
    }
    if (config_status == syrius_orbit::RuntimeConfigLoadStatus::kError) {
        PLOGE << "Configuration error: " << config_message;
        return 1;
    }

    std::signal(SIGINT, &on_signal);
    std::signal(SIGTERM, &on_signal);

    syrius_orbit::Daemon daemon(config);
    int daemon_exit_code = 1;
    std::atomic_bool daemon_finished{false};

    std::thread daemon_thread([&daemon, &daemon_exit_code, &daemon_finished] {
        daemon_exit_code = daemon.run();
        daemon_finished.store(true, std::memory_order_relaxed);
    });

    while (!g_signal_received.load(std::memory_order_relaxed) && !daemon_finished.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!daemon_finished.load(std::memory_order_relaxed)) {
        daemon.stop();
    }
    daemon_thread.join();

    PLOGI << "Daemon exited with code: " << daemon_exit_code;

    return daemon_exit_code;
}
