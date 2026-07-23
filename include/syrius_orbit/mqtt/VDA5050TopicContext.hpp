#pragma once

#include <string>

namespace syrius_orbit {

struct VDA5050TopicContext {
    std::string interfaceName;
    std::string majorVersion;
    std::string manufacturer;
    std::string serialNumber;
};

}  // namespace syrius_orbit
