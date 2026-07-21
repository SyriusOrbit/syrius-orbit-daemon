#pragma once

#include "common.hpp"
#include "node_position.h"
#include "visualization.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050 {

enum class MapStatus {
  ENABLED,
  DISABLED,
};

NLOHMANN_JSON_SERIALIZE_ENUM(MapStatus, {
                                            {MapStatus::ENABLED, "ENABLED"},
                                            {MapStatus::DISABLED, "DISABLED"},
                                        });

enum class ZoneSetStatus {
  ENABLED,
  DISABLED,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ZoneSetStatus,
                             {
                                 {ZoneSetStatus::ENABLED, "ENABLED"},
                                 {ZoneSetStatus::DISABLED, "DISABLED"},
                             });

enum class ZoneRequestType {
  ACCESS,
  REPLANNING,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ZoneRequestType,
                             {
                                 {ZoneRequestType::ACCESS, "ACCESS"},
                                 {ZoneRequestType::REPLANNING, "REPLANNING"},
                             });

enum class EdgeRequestType {
  CORRIDOR,
};

NLOHMANN_JSON_SERIALIZE_ENUM(EdgeRequestType,
                             {
                                 {EdgeRequestType::CORRIDOR, "CORRIDOR"},
                             });

enum class RequestStatus {
  REQUESTED,
  GRANTED,
  REVOKED,
  EXPIRED,
};

NLOHMANN_JSON_SERIALIZE_ENUM(RequestStatus,
                             {
                                 {RequestStatus::REQUESTED, "REQUESTED"},
                                 {RequestStatus::GRANTED, "GRANTED"},
                                 {RequestStatus::REVOKED, "REVOKED"},
                                 {RequestStatus::EXPIRED, "EXPIRED"},
                             });

enum class ActionStatus {
  WAITING,
  INITIALIZING,
  RUNNING,
  PAUSED,
  RETRIABLE,
  FINISHED,
  FAILED,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ActionStatus,
                             {
                                 {ActionStatus::WAITING, "WAITING"},
                                 {ActionStatus::INITIALIZING, "INITIALIZING"},
                                 {ActionStatus::RUNNING, "RUNNING"},
                                 {ActionStatus::PAUSED, "PAUSED"},
                                 {ActionStatus::RETRIABLE, "RETRIABLE"},
                                 {ActionStatus::FINISHED, "FINISHED"},
                                 {ActionStatus::FAILED, "FAILED"},
                             });

enum class OperatingMode {
  STARTUP,
  AUTOMATIC,
  SEMIAUTOMATIC,
  INTERVENED,
  MANUAL,
  SERVICE,
  TEACH_IN,
};

NLOHMANN_JSON_SERIALIZE_ENUM(OperatingMode,
                             {
                                 {OperatingMode::STARTUP, "STARTUP"},
                                 {OperatingMode::AUTOMATIC, "AUTOMATIC"},
                                 {OperatingMode::SEMIAUTOMATIC,
                                  "SEMIAUTOMATIC"},
                                 {OperatingMode::INTERVENED, "INTERVENED"},
                                 {OperatingMode::MANUAL, "MANUAL"},
                                 {OperatingMode::SERVICE, "SERVICE"},
                                 {OperatingMode::TEACH_IN, "TEACH_IN"},
                             });

enum class ErrorLevel {
  WARNING,
  URGENT,
  CRITICAL,
  FATAL,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ErrorLevel, {
                                             {ErrorLevel::WARNING, "WARNING"},
                                             {ErrorLevel::URGENT, "URGENT"},
                                             {ErrorLevel::CRITICAL, "CRITICAL"},
                                             {ErrorLevel::FATAL, "FATAL"},
                                         });

enum class InfoLevel {
  INFO,
  DEBUG,
};

NLOHMANN_JSON_SERIALIZE_ENUM(InfoLevel, {
                                            {InfoLevel::INFO, "INFO"},
                                            {InfoLevel::DEBUG, "DEBUG"},
                                        });

enum class ActiveEmergencyStop {
  MANUAL,
  REMOTE,
  NONE,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ActiveEmergencyStop,
                             {
                                 {ActiveEmergencyStop::MANUAL, "MANUAL"},
                                 {ActiveEmergencyStop::REMOTE, "REMOTE"},
                                 {ActiveEmergencyStop::NONE, "NONE"},
                             });

struct Map {
  std::string mapId{};
  std::string mapVersion{};
  std::optional<std::string> mapDescriptor{};
  MapStatus mapStatus{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Map, mapId, mapVersion,
                                                mapDescriptor, mapStatus);

struct ZoneSet {
  std::string zoneSetId{};
  std::string mapId{};
  ZoneSetStatus zoneSetStatus{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ZoneSet, zoneSetId, mapId,
                                                zoneSetStatus);

struct NodeState {
  std::string nodeId{};
  std::int64_t sequenceId{};
  std::optional<std::string> nodeDescriptor{};
  bool released{};
  std::optional<NodePosition> nodePosition{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(NodeState, nodeId, sequenceId,
                                                nodeDescriptor, released,
                                                nodePosition);

struct EdgeState {
  std::string edgeId{};
  std::int64_t sequenceId{};
  std::optional<std::string> edgeDescriptor{};
  bool released{};
  std::optional<Trajectory> trajectory{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(EdgeState, edgeId, sequenceId,
                                                edgeDescriptor, released,
                                                trajectory);

struct LoadLoadDimensions {
  double length{};
  double width{};
  std::optional<double> height{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LoadLoadDimensions, length,
                                                width, height);

struct Load {
  std::optional<std::string> loadId{};
  std::optional<std::string> loadType{};
  std::optional<std::string> loadPosition{};
  std::optional<BoundingBox> boundingBoxReference{};
  std::optional<LoadLoadDimensions> loadDimensions{};
  std::optional<double> weight{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Load, loadId, loadType,
                                                loadPosition,
                                                boundingBoxReference,
                                                loadDimensions, weight);

struct ZoneRequest {
  std::string requestId{};
  ZoneRequestType requestType{};
  std::string zoneId{};
  std::string zoneSetId{};
  RequestStatus requestStatus{};
  std::optional<Trajectory> trajectory{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ZoneRequest, requestId,
                                                requestType, zoneId, zoneSetId,
                                                requestStatus, trajectory);

struct EdgeRequest {
  std::string requestId{};
  EdgeRequestType requestType{};
  std::string edgeId{};
  std::int64_t sequenceId{};
  RequestStatus requestStatus{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(EdgeRequest, requestId,
                                                requestType, edgeId, sequenceId,
                                                requestStatus);

struct ActionState {
  std::string actionId{};
  std::optional<std::string> actionType{};
  std::optional<std::string> actionDescriptor{};
  ActionStatus actionStatus{};
  std::optional<std::string> actionResult{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ActionState, actionId,
                                                actionType, actionDescriptor,
                                                actionStatus, actionResult);

struct PowerSupply {
  double stateOfCharge{};
  std::optional<double> batteryVoltage{};
  std::optional<double> batteryCurrent{};
  std::optional<double> batteryHealth{};
  bool charging{};
  std::optional<double> range{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(PowerSupply, stateOfCharge,
                                                batteryVoltage, batteryCurrent,
                                                batteryHealth, charging, range);

struct Translation {
  std::string translationKey{};
  std::string translationValue{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Translation, translationKey,
                                                translationValue);

struct Reference {
  std::string referenceKey{};
  std::string referenceValue{};
};

struct Error {
  std::string errorType{};
  std::optional<std::vector<Reference>> errorReferences{};
  std::optional<std::string> errorDescription{};
  std::optional<std::vector<Translation>> errorDescriptionTranslations{};
  std::optional<std::string> errorHint{};
  std::optional<std::vector<Translation>> errorHintTranslations{};
  ErrorLevel errorLevel{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    Error, errorType, errorReferences, errorDescription,
    errorDescriptionTranslations, errorHint, errorHintTranslations, errorLevel);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Reference, referenceKey,
                                                referenceValue);

struct Info {
  std::string infoType{};
  std::optional<std::vector<Reference>> infoReferences{};
  std::optional<std::string> infoDescriptor{};
  InfoLevel infoLevel{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Info, infoType, infoReferences,
                                                infoDescriptor, infoLevel);

struct SafetyState {
  ActiveEmergencyStop activeEmergencyStop{};
  bool fieldViolation{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SafetyState,
                                                activeEmergencyStop,
                                                fieldViolation);

struct State {
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
  std::optional<Velocity> velocity{};
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
  OperatingMode operatingMode{};
  std::vector<Error> errors{};
  std::optional<std::vector<Info>> information{};
  SafetyState safetyState{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    State, headerId, timestamp, version, manufacturer, serialNumber, maps,
    zoneSets, orderId, orderUpdateId, lastNodeId, lastNodeSequenceId,
    nodeStates, edgeStates, plannedPath, intermediatePath, mobileRobotPosition,
    velocity, loads, driving, paused, newBaseRequest, zoneRequests,
    edgeRequests, distanceSinceLastNode, actionStates, instantActionStates,
    zoneActionStates, powerSupply, operatingMode, errors, information,
    safetyState);

} // namespace syrius_orbit::vda5050
