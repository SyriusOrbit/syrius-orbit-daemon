#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050::schema::state {

struct Map {
  std::string mapId{};
  std::string mapVersion{};
  std::optional<std::string> mapDescriptor{};
  std::string mapStatus{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Map, mapId, mapVersion, mapDescriptor, mapStatus);

struct ZoneSet {
  std::string zoneSetId{};
  std::string mapId{};
  std::string zoneSetStatus{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ZoneSet, zoneSetId, mapId, zoneSetStatus);

struct NodeStateNodePosition {
  double x{};
  double y{};
  std::optional<double> theta{};
  std::string mapId{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(NodeStateNodePosition, x, y, theta, mapId);

struct NodeState {
  std::string nodeId{};
  std::int64_t sequenceId{};
  std::optional<std::string> nodeDescriptor{};
  bool released{};
  std::optional<NodeStateNodePosition> nodePosition{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(NodeState, nodeId, sequenceId, nodeDescriptor, released, nodePosition);

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

struct EdgeState {
  std::string edgeId{};
  std::int64_t sequenceId{};
  std::optional<std::string> edgeDescriptor{};
  bool released{};
  std::optional<Trajectory> trajectory{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(EdgeState, edgeId, sequenceId, edgeDescriptor, released, trajectory);

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

struct StateMessageVelocity {
  std::optional<double> vx{};
  std::optional<double> vy{};
  std::optional<double> omega{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(StateMessageVelocity, vx, vy, omega);

struct LoadBoundingBoxReference {
  double x{};
  double y{};
  double z{};
  std::optional<double> theta{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LoadBoundingBoxReference, x, y, z, theta);

struct LoadLoadDimensions {
  double length{};
  double width{};
  std::optional<double> height{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LoadLoadDimensions, length, width, height);

struct Load {
  std::optional<std::string> loadId{};
  std::optional<std::string> loadType{};
  std::optional<std::string> loadPosition{};
  std::optional<LoadBoundingBoxReference> boundingBoxReference{};
  std::optional<LoadLoadDimensions> loadDimensions{};
  std::optional<double> weight{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Load, loadId, loadType, loadPosition, boundingBoxReference, loadDimensions, weight);

struct ZoneRequest {
  std::string requestId{};
  std::string requestType{};
  std::string zoneId{};
  std::string zoneSetId{};
  std::string requestStatus{};
  std::optional<Trajectory> trajectory{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ZoneRequest, requestId, requestType, zoneId, zoneSetId, requestStatus, trajectory);

struct EdgeRequest {
  std::string requestId{};
  std::string requestType{};
  std::string edgeId{};
  std::int64_t sequenceId{};
  std::string requestStatus{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(EdgeRequest, requestId, requestType, edgeId, sequenceId, requestStatus);

struct ActionState {
  std::string actionId{};
  std::optional<std::string> actionType{};
  std::optional<std::string> actionDescriptor{};
  std::string actionStatus{};
  std::optional<std::string> actionResult{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ActionState, actionId, actionType, actionDescriptor, actionStatus, actionResult);

struct PowerSupply {
  double stateOfCharge{};
  std::optional<double> batteryVoltage{};
  std::optional<double> batteryCurrent{};
  std::optional<double> batteryHealth{};
  bool charging{};
  std::optional<double> range{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(PowerSupply, stateOfCharge, batteryVoltage, batteryCurrent, batteryHealth, charging, range);

struct ErrorErrorReferencesItem {
  std::string referenceKey{};
  std::string referenceValue{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ErrorErrorReferencesItem, referenceKey, referenceValue);

struct Translation {
  std::string translationKey{};
  std::string translationValue{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Translation, translationKey, translationValue);

struct Error {
  std::string errorType{};
  std::optional<std::vector<ErrorErrorReferencesItem>> errorReferences{};
  std::optional<std::string> errorDescription{};
  std::optional<std::vector<Translation>> errorDescriptionTranslations{};
  std::optional<std::string> errorHint{};
  std::optional<std::vector<Translation>> errorHintTranslations{};
  std::string errorLevel{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Error, errorType, errorReferences, errorDescription, errorDescriptionTranslations, errorHint, errorHintTranslations, errorLevel);

struct InfoInfoReferencesItem {
  std::string referenceKey{};
  std::string referenceValue{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(InfoInfoReferencesItem, referenceKey, referenceValue);

struct Info {
  std::string infoType{};
  std::optional<std::vector<InfoInfoReferencesItem>> infoReferences{};
  std::optional<std::string> infoDescriptor{};
  std::string infoLevel{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Info, infoType, infoReferences, infoDescriptor, infoLevel);

struct SafetyState {
  std::string activeEmergencyStop{};
  bool fieldViolation{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SafetyState, activeEmergencyStop, fieldViolation);

struct StateMessage {
  std::int64_t headerId{};
  std::string timestamp{};
  std::string version{};
  std::string manufacturer{};
  std::string serialNumber{};
  std::optional<std::vector<Map>> maps{};
  std::optional<std::vector<ZoneSet>> zoneSets{};
  std::string orderId{};
  std::int64_t orderUpdateId{};
  std::string lastNodeId{};
  std::int64_t lastNodeSequenceId{};
  std::vector<NodeState> nodeStates{};
  std::vector<EdgeState> edgeStates{};
  std::optional<PlannedPath> plannedPath{};
  std::optional<IntermediatePath> intermediatePath{};
  std::optional<MobileRobotPosition> mobileRobotPosition{};
  std::optional<StateMessageVelocity> velocity{};
  std::optional<std::vector<Load>> loads{};
  bool driving{};
  std::optional<bool> paused{};
  std::optional<bool> newBaseRequest{};
  std::optional<std::vector<ZoneRequest>> zoneRequests{};
  std::optional<std::vector<EdgeRequest>> edgeRequests{};
  std::optional<double> distanceSinceLastNode{};
  std::vector<ActionState> actionStates{};
  std::vector<ActionState> instantActionStates{};
  std::optional<std::vector<ActionState>> zoneActionStates{};
  PowerSupply powerSupply{};
  std::string operatingMode{};
  std::vector<Error> errors{};
  std::optional<std::vector<Info>> information{};
  SafetyState safetyState{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(StateMessage, headerId, timestamp, version, manufacturer, serialNumber, maps, zoneSets, orderId, orderUpdateId, lastNodeId, lastNodeSequenceId, nodeStates, edgeStates, plannedPath, intermediatePath, mobileRobotPosition, velocity, loads, driving, paused, newBaseRequest, zoneRequests, edgeRequests, distanceSinceLastNode, actionStates, instantActionStates, zoneActionStates, powerSupply, operatingMode, errors, information, safetyState);

} // namespace syrius_orbit::vda5050::schema::state
