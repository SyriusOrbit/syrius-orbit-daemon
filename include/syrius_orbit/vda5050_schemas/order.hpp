#pragma once

#include "action.h"
#include "node_position.h"
#include "trajectory.h"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050 {

enum class OrientationType {
  GLOBAL,
  TANGENTIAL,
};

NLOHMANN_JSON_SERIALIZE_ENUM(OrientationType,
                             {
                                 {OrientationType::GLOBAL, "GLOBAL"},
                                 {OrientationType::TANGENTIAL, "TANGENTIAL"},
                             });

enum class ReferencePoint {
  KINEMATIC_CENTER,
  CONTOUR,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ReferencePoint,
                             {
                                 {ReferencePoint::KINEMATIC_CENTER,
                                  "KINEMATIC_CENTER"},
                                 {ReferencePoint::CONTOUR, "CONTOUR"},
                             });

enum class ReleaseLossBehavior {
  STOP,
  RETURN,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ReleaseLossBehavior,
                             {
                                 {ReleaseLossBehavior::STOP, "STOP"},
                                 {ReleaseLossBehavior::RETURN, "RETURN"},
                             });

struct Node {
  std::string nodeId{};
  std::int64_t sequenceId{};
  std::optional<std::string> nodeDescriptor{};
  bool released{};
  std::optional<NodePosition> nodePosition{};
  std::vector<Action> actions{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Node, nodeId, sequenceId,
                                                nodeDescriptor, released,
                                                nodePosition, actions);

struct Corridor {
  double leftWidth{};
  double rightWidth{};
  std::optional<ReferencePoint> corridorReferencePoint{};
  std::optional<bool> releaseRequired{};
  std::optional<ReleaseLossBehavior> releaseLossBehavior{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Corridor, leftWidth, rightWidth,
                                                corridorReferencePoint,
                                                releaseRequired,
                                                releaseLossBehavior);

struct Edge {
  std::string edgeId{};
  std::int64_t sequenceId{};
  std::optional<std::string> edgeDescriptor{};
  bool released{};
  std::optional<double> maximumSpeed{};
  std::optional<double> maximumMobileRobotHeight{};
  std::optional<double> minimumLoadHandlingDeviceHeight{};
  std::optional<double> orientation{};
  std::optional<OrientationType> orientationType{};
  std::optional<std::string> direction{}; // left, right, straight, 580Hz. for
                                          // physical line guided mobile robots
  std::optional<bool> reachOrientationBeforeEntering{};
  std::optional<double> maxRotationSpeed{};
  std::optional<Trajectory> trajectory{};
  std::optional<double> length{};
  std::optional<Corridor> corridor{};
  std::vector<Action> actions{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    Edge, edgeId, sequenceId, edgeDescriptor, released, maximumSpeed,
    maximumMobileRobotHeight, minimumLoadHandlingDeviceHeight, orientation,
    orientationType, direction, reachOrientationBeforeEntering,
    maxRotationSpeed, trajectory, length, corridor, actions);

struct Order {
  std::int64_t headerId{};
  std::string timestamp{};
  std::string version{};
  std::string manufacturer{};
  std::string serialNumber{};
  std::string orderId{};
  std::int64_t orderUpdateId{};
  std::optional<std::string> orderDescription{};
  std::vector<Node> nodes{};
  std::vector<Edge> edges{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Order, headerId, timestamp,
                                                version, manufacturer,
                                                serialNumber, orderId,
                                                orderUpdateId, orderDescription,
                                                nodes, edges);

} // namespace syrius_orbit::vda5050
