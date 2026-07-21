#pragma once

#include <nlohmann/json.hpp>

namespace syrius_orbit::vda5050 {

struct AllowedDeviationXY {
  double a{};
  double b{};
  double theta{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AllowedDeviationXY, a, b,
                                                theta);

struct NodePosition {
  double x{};
  double y{};
  std::optional<double> theta{};
  std::optional<AllowedDeviationXY> allowedDeviationXY{};
  std::optional<double> allowedDeviationTheta{};
  std::string mapId{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(NodePosition, x, y, theta,
                                                allowedDeviationXY,
                                                allowedDeviationTheta, mapId);
} // namespace syrius_orbit::vda5050