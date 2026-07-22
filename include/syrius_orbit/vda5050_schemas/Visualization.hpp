#pragma once

#include "Trajectory.h"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace syrius_orbit::vda5050 {

struct PlannedPath {
  Trajectory trajectory{};
  std::optional<std::vector<std::string>> traversedNodes{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(PlannedPath, trajectory,
                                                traversedNodes);

struct WayPoint {
  double x{};
  double y{};
  std::optional<double> theta{};
  std::string eta{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(WayPoint, x, y, theta, eta);

struct IntermediatePath {
  std::vector<WayPoint> polyline{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(IntermediatePath, polyline);

struct MobileRobotPosition {
  double x{};
  double y{};
  double theta{};
  std::string mapId{};
  bool localized{};
  std::optional<double> localizationScore{};
  std::optional<double> deviationRange{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MobileRobotPosition, x, y,
                                                theta, mapId, localized,
                                                localizationScore,
                                                deviationRange);

struct Velocity {
  std::optional<double> vx{};
  std::optional<double> vy{};
  std::optional<double> omega{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Velocity, vx, vy, omega);

struct Visualization {
  std::int64_t headerId{};
  std::string timestamp{};
  std::string version{};
  std::string manufacturer{};
  std::string serialNumber{};
  std::int64_t referenceStateHeaderId{};
  std::optional<PlannedPath> plannedPath{};
  std::optional<IntermediatePath> intermediatePath{};
  std::optional<MobileRobotPosition> mobileRobotPosition{};
  std::optional<Velocity> velocity{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Visualization, headerId,
                                                timestamp, version,
                                                manufacturer, serialNumber,
                                                referenceStateHeaderId,
                                                plannedPath, intermediatePath,
                                                mobileRobotPosition, velocity);

} // namespace syrius_orbit::vda5050
