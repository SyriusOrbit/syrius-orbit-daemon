#pragma once

#include "action.h"
#include "common.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050 {

struct InstantActions {
  std::int64_t headerId{};
  std::string timestamp{};
  std::string version{};
  std::string manufacturer{};
  std::string serialNumber{};
  std::vector<Action> actions{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(InstantActions, headerId,
                                                timestamp, version,
                                                manufacturer, serialNumber,
                                                actions);

} // namespace syrius_orbit::vda5050
