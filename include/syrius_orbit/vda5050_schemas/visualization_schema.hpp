#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050::schema::visualization {

struct TrajectoryControlPointsItem {
  double x{};
  double y{};
  std::optional<double> weight{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(TrajectoryControlPointsItem, x, y, weight);

struct Trajectory {
  std::optional<std::int64_t> degree{};
  std::optional<std::vector<double>> knotVector{};
  std::vector<TrajectoryControlPointsItem> controlPoints{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Trajectory, degree, knotVector, controlPoints);

struct PlannedPath {
  Trajectory trajectory{};
  std::optional<std::vector<std::string>> traversedNodes{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(PlannedPath, trajectory, traversedNodes);

struct IntermediatePathPolylineItem {
  double x{};
  double y{};
  std::optional<double> theta{};
  std::string eta{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(IntermediatePathPolylineItem, x, y, theta, eta);

struct IntermediatePath {
  std::vector<IntermediatePathPolylineItem> polyline{};
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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MobileRobotPosition, x, y, theta, mapId, localized, localizationScore, deviationRange);

struct Velocity {
  std::optional<double> vx{};
  std::optional<double> vy{};
  std::optional<double> omega{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Velocity, vx, vy, omega);

struct VisualizationMessage {
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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualizationMessage, headerId, timestamp, version, manufacturer, serialNumber, referenceStateHeaderId, plannedPath, intermediatePath, mobileRobotPosition, velocity);

} // namespace syrius_orbit::vda5050::schema::visualization
