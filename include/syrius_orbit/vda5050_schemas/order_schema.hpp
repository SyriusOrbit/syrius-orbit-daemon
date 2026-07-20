#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050::schema::order {

struct NodeNodePositionAllowedDeviationXY {
  double a{};
  double b{};
  double theta{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(NodeNodePositionAllowedDeviationXY, a, b, theta);

struct NodeNodePosition {
  double x{};
  double y{};
  std::optional<double> theta{};
  std::optional<NodeNodePositionAllowedDeviationXY> allowedDeviationXY{};
  std::optional<double> allowedDeviationTheta{};
  std::string mapId{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(NodeNodePosition, x, y, theta, allowedDeviationXY, allowedDeviationTheta, mapId);

struct ActionActionParametersItem {
  std::string key{};
  nlohmann::json value{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ActionActionParametersItem, key, value);

struct Action {
  std::string actionType{};
  std::string actionId{};
  std::optional<std::string> actionDescriptor{};
  std::string blockingType{};
  std::optional<std::vector<ActionActionParametersItem>> actionParameters{};
  std::optional<bool> retriable{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Action, actionType, actionId, actionDescriptor, blockingType, actionParameters, retriable);

struct Node {
  std::string nodeId{};
  std::int64_t sequenceId{};
  std::optional<std::string> nodeDescriptor{};
  bool released{};
  std::optional<NodeNodePosition> nodePosition{};
  std::vector<Action> actions{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Node, nodeId, sequenceId, nodeDescriptor, released, nodePosition, actions);

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

struct Corridor {
  double leftWidth{};
  double rightWidth{};
  std::optional<std::string> corridorReferencePoint{};
  std::optional<bool> releaseRequired{};
  std::optional<std::string> releaseLossBehavior{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Corridor, leftWidth, rightWidth, corridorReferencePoint, releaseRequired, releaseLossBehavior);

struct Edge {
  std::string edgeId{};
  std::int64_t sequenceId{};
  std::optional<std::string> edgeDescriptor{};
  bool released{};
  std::optional<double> maximumSpeed{};
  std::optional<double> maximumMobileRobotHeight{};
  std::optional<double> minimumLoadHandlingDeviceHeight{};
  std::optional<double> orientation{};
  std::optional<std::string> orientationType{};
  std::optional<std::string> direction{};
  std::optional<bool> reachOrientationBeforeEntering{};
  std::optional<double> maxRotationSpeed{};
  std::optional<Trajectory> trajectory{};
  std::optional<double> length{};
  std::optional<Corridor> corridor{};
  std::vector<Action> actions{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Edge, edgeId, sequenceId, edgeDescriptor, released, maximumSpeed, maximumMobileRobotHeight, minimumLoadHandlingDeviceHeight, orientation, orientationType, direction, reachOrientationBeforeEntering, maxRotationSpeed, trajectory, length, corridor, actions);

struct OrderMessage {
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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(OrderMessage, headerId, timestamp, version, manufacturer, serialNumber, orderId, orderUpdateId, orderDescription, nodes, edges);

} // namespace syrius_orbit::vda5050::schema::order
