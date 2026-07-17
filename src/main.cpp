#include "syrius_orbit/daemon.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
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

int main() {
    static plog::ConsoleAppender<plog::TxtFormatter> console_appender;
    plog::init(plog::info, &console_appender);

    std::signal(SIGINT, &on_signal);
    std::signal(SIGTERM, &on_signal);

    syrius_orbit::Daemon daemon;
    int daemon_exit_code = 1;

    std::thread daemon_thread([&daemon, &daemon_exit_code] {
        daemon_exit_code = daemon.run();
    });

    while (!g_signal_received.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    daemon.stop();
    daemon_thread.join();

    PLOGI << "Daemon exited with code: " << daemon_exit_code;

    return daemon_exit_code;
}
