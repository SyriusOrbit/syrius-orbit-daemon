#pragma once

#include <nlohmann/json.hpp>

namespace syrius_orbit::vda5050 {

struct ControlPoint {
  double x{};
  double y{};
  std::optional<double> weight{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ControlPoint, x, y, weight);

struct Trajectory {
  std::optional<std::int32_t> degree{};
  std::optional<std::vector<double>> knotVector{};
  std::vector<ControlPoint> controlPoints{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Trajectory, degree, knotVector,
                                                controlPoints);

} // namespace syrius_orbit::vda5050