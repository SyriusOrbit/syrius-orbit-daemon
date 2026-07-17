#pragma once

namespace syrius_orbit {

class Daemon {
public:
    [[nodiscard]] int run() const;
    void stop();

private:
    bool stop_requested_{false};
};

}  // namespace syrius_orbit
