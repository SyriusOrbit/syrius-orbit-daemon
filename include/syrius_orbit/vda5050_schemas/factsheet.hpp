#pragma once

#include "common.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050 {

enum class SupportedZonesItem {
  BLOCKED,
  LINE_GUIDED,
  RELEASE,
  COORDINATED_REPLANNING,
  SPEED_LIMIT,
  ACTION,
  PRIORITY,
  PENALTY,
  DIRECTED,
  BIDIRECTED,
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    SupportedZonesItem,
    {
        {SupportedZonesItem::BLOCKED, "BLOCKED"},
        {SupportedZonesItem::LINE_GUIDED, "LINE_GUIDED"},
        {SupportedZonesItem::RELEASE, "RELEASE"},
        {SupportedZonesItem::COORDINATED_REPLANNING, "COORDINATED_REPLANNING"},
        {SupportedZonesItem::SPEED_LIMIT, "SPEED_LIMIT"},
        {SupportedZonesItem::ACTION, "ACTION"},
        {SupportedZonesItem::PRIORITY, "PRIORITY"},
        {SupportedZonesItem::PENALTY, "PENALTY"},
        {SupportedZonesItem::DIRECTED, "DIRECTED"},
        {SupportedZonesItem::BIDIRECTED, "BIDIRECTED"},
    });

enum class SupportType {
  SUPPORTED,
  REQUIRED,
};

NLOHMANN_JSON_SERIALIZE_ENUM(SupportType,
                             {
                                 {SupportType::SUPPORTED, "SUPPORTED"},
                                 {SupportType::REQUIRED, "REQUIRED"},
                             });

enum class ActionScope {
  INSTANT,
  NODE,
  EDGE,
  ZONE,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ActionScope, {
                                              {ActionScope::INSTANT, "INSTANT"},
                                              {ActionScope::NODE, "NODE"},
                                              {ActionScope::EDGE, "EDGE"},
                                              {ActionScope::ZONE, "ZONE"},
                                          });

enum class ValueDataType {
  BOOL,
  NUMBER,
  INTEGER,
  STRING,
  OBJECT,
  ARRAY,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ValueDataType,
                             {
                                 {ValueDataType::BOOL, "BOOL"},
                                 {ValueDataType::NUMBER, "NUMBER"},
                                 {ValueDataType::INTEGER, "INTEGER"},
                                 {ValueDataType::STRING, "STRING"},
                                 {ValueDataType::OBJECT, "OBJECT"},
                                 {ValueDataType::ARRAY, "ARRAY"},
                             });

struct TypeSpecification {
  std::string seriesName{};
  std::optional<std::string> seriesDescription{};
  std::string mobileRobotKinematics{};
  std::string mobileRobotClass{};
  double maximumLoadMass{};
  std::vector<std::string> localizationTypes{};
  std::vector<std::string> navigationTypes{};
  std::optional<std::vector<SupportedZonesItem>> supportedZones{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    TypeSpecification, seriesName, seriesDescription, mobileRobotKinematics,
    mobileRobotClass, maximumLoadMass, localizationTypes, navigationTypes,
    supportedZones);

struct PhysicalParameters {
  double minimumSpeed{};
  double maximumSpeed{};
  std::optional<double> minimumAngularSpeed{};
  std::optional<double> maximumAngularSpeed{};
  double maximumAcceleration{};
  double maximumDeceleration{};
  double minimumHeight{};
  double maximumHeight{};
  double width{};
  double length{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    PhysicalParameters, minimumSpeed, maximumSpeed, minimumAngularSpeed,
    maximumAngularSpeed, maximumAcceleration, maximumDeceleration,
    minimumHeight, maximumHeight, width, length);

struct MaximumStringLengths {
  std::optional<std::int64_t> maximumMessageLength{};
  std::optional<std::int64_t> maximumTopicSerialLength{};
  std::optional<std::int64_t> maximumTopicElementLength{};
  std::optional<std::int64_t> maximumIdLength{};
  std::optional<bool> idNumericalOnly{};
  std::optional<std::int64_t> maximumLoadIdLength{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    MaximumStringLengths, maximumMessageLength, maximumTopicSerialLength,
    maximumTopicElementLength, maximumIdLength, idNumericalOnly,
    maximumLoadIdLength);

struct MaximumArrayLengths {
  std::optional<std::int64_t> order_nodes{};
  std::optional<std::int64_t> order_edges{};
  std::optional<std::int64_t> node_actions{};
  std::optional<std::int64_t> edge_actions{};
  std::optional<std::int64_t> actions_actionsParameters{};
  std::optional<std::int64_t> instantActions{};
  std::optional<std::int64_t> trajectory_knotVector{};
  std::optional<std::int64_t> trajectory_controlPoints{};
  std::optional<std::int64_t> zoneSet_zones{};
  std::optional<std::int64_t> state_nodeStates{};
  std::optional<std::int64_t> state_edgeStates{};
  std::optional<std::int64_t> state_loads{};
  std::optional<std::int64_t> state_actionStates{};
  std::optional<std::int64_t> state_instantActionStates{};
  std::optional<std::int64_t> state_zoneActionStates{};
  std::optional<std::int64_t> state_errors{};
  std::optional<std::int64_t> state_information{};
  std::optional<std::int64_t> error_errorReferences{};
  std::optional<std::int64_t> information_infoReferences{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    MaximumArrayLengths, order_nodes, order_edges, node_actions, edge_actions,
    actions_actionsParameters, instantActions, trajectory_knotVector,
    trajectory_controlPoints, zoneSet_zones, state_nodeStates, state_edgeStates,
    state_loads, state_actionStates, state_instantActionStates,
    state_zoneActionStates, state_errors, state_information,
    error_errorReferences, information_infoReferences);

struct Timing {
  double minimumOrderInterval{};
  double minimumStateInterval{};
  std::optional<double> defaultStateInterval{};
  std::optional<double> visualizationInterval{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Timing, minimumOrderInterval,
                                                minimumStateInterval,
                                                defaultStateInterval,
                                                visualizationInterval);

struct ProtocolLimits {
  MaximumStringLengths maximumStringLengths{};
  MaximumArrayLengths maximumArrayLengths{};
  Timing timing{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ProtocolLimits,
                                                maximumStringLengths,
                                                maximumArrayLengths, timing);

struct OptionalParameter {
  std::string parameter{};
  SupportType support{};
  std::optional<std::string> description{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(OptionalParameter, parameter,
                                                support, description);

struct ActionParameterSchema {
  std::string key{};
  ValueDataType valueDataType{};
  std::optional<std::string> description{};
  std::optional<bool> isOptional{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ActionParameterSchema, key,
                                                valueDataType, description,
                                                isOptional);

struct MobileRobotAction {
  std::string actionType{};
  std::optional<std::string> actionDescription{};
  std::vector<ActionScope> actionScopes{};
  std::optional<std::vector<ActionParameterSchema>> actionParameters{};
  std::optional<std::string> actionResult{};
  std::optional<std::vector<BlockingType>> blockingTypes{};
  bool pauseAllowed{};
  bool cancelAllowed{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MobileRobotAction, actionType,
                                                actionDescription, actionScopes,
                                                actionParameters, actionResult,
                                                blockingTypes, pauseAllowed,
                                                cancelAllowed);

struct ProtocolFeatures {
  std::vector<OptionalParameter> optionalParameters{};
  std::vector<MobileRobotAction> mobileRobotActions{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ProtocolFeatures,
                                                optionalParameters,
                                                mobileRobotActions);

struct Position {
  double x{};
  double y{};
  std::optional<double> theta{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Position, x, y, theta);

struct WheelDefinition {
  std::string type{};
  bool isActiveDriven{};
  bool isActiveSteered{};
  Position position{};
  double diameter{};
  double width{};
  std::optional<double> centerDisplacement{};
  std::optional<std::string> constraints{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(WheelDefinition, type,
                                                isActiveDriven, isActiveSteered,
                                                position, diameter, width,
                                                centerDisplacement,
                                                constraints);

struct Envelope2d {
  std::string envelope2dId{};
  std::vector<Vertex> vertices{};
  std::optional<std::string> description{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Envelope2d, envelope2dId,
                                                vertices, description);

struct Envelope3d {
  std::string envelope3dId{};
  std::string format{};
  std::optional<nlohmann::json> data{};
  std::optional<std::string> url{};
  std::optional<std::string> description{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Envelope3d, envelope3dId,
                                                format, data, url, description);

struct MobileRobotGeometry {
  std::optional<std::vector<WheelDefinition>> wheelDefinitions{};
  std::optional<std::vector<Envelope2d>> envelopes2d{};
  std::optional<std::vector<Envelope3d>> envelopes3d{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MobileRobotGeometry,
                                                wheelDefinitions, envelopes2d,
                                                envelopes3d);

struct LoadDimensions {
  double length{};
  double width{};
  std::optional<double> height{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LoadDimensions, length, width,
                                                height);

struct LoadSet {
  std::string setName{};
  std::string loadType{};
  std::optional<std::vector<std::string>> loadPositions{};
  std::optional<BoundingBox> boundingBoxReference{};
  std::optional<LoadDimensions> loadDimensions{};
  std::optional<double> maximumWeight{};
  std::optional<double> minimumLoadhandlingHeight{};
  std::optional<double> maximumLoadhandlingHeight{};
  std::optional<double> minimumLoadhandlingDepth{};
  std::optional<double> maximumLoadhandlingDepth{};
  std::optional<double> minimumLoadhandlingTilt{};
  std::optional<double> maximumLoadhandlingTilt{};
  std::optional<double> maximumSpeed{};
  std::optional<double> maximumAcceleration{};
  std::optional<double> maximumDeceleration{};
  std::optional<double> pickTime{};
  std::optional<double> dropTime{};
  std::optional<std::string> description{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    LoadSet, setName, loadType, loadPositions, boundingBoxReference,
    loadDimensions, maximumWeight, minimumLoadhandlingHeight,
    maximumLoadhandlingHeight, minimumLoadhandlingDepth,
    maximumLoadhandlingDepth, minimumLoadhandlingTilt, maximumLoadhandlingTilt,
    maximumSpeed, maximumAcceleration, maximumDeceleration, pickTime, dropTime,
    description);

struct LoadSpecification {
  std::optional<std::vector<std::string>> loadPositions{};
  std::optional<std::vector<LoadSet>> loadSets{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LoadSpecification,
                                                loadPositions, loadSets);

struct Version {
  std::string key{};
  std::string value{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Version, key, value);

struct Network {
  std::optional<std::vector<std::string>> dnsServers{};
  std::optional<std::vector<std::string>> ntpServers{};
  std::optional<std::string> localIpAddress{};
  std::optional<std::string> netmask{};
  std::optional<std::string> defaultGateway{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Network, dnsServers, ntpServers,
                                                localIpAddress, netmask,
                                                defaultGateway);

struct BatteryCharging {
  std::optional<double> criticalLowChargingLevel{};
  std::optional<double> minimumDesiredChargingLevel{};
  std::optional<double> maximumDesiredChargingLevel{};
  std::optional<double> minimumChargingTime{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(BatteryCharging,
                                                criticalLowChargingLevel,
                                                minimumDesiredChargingLevel,
                                                maximumDesiredChargingLevel,
                                                minimumChargingTime);

struct MobileRobotConfiguration {
  std::optional<std::vector<Version>> versions{};
  std::optional<Network> network{};
  std::optional<BatteryCharging> batteryCharging{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MobileRobotConfiguration,
                                                versions, network,
                                                batteryCharging);

struct Factsheet {
  std::int64_t headerId{};
  std::string timestamp{};
  std::string version{};
  std::string manufacturer{};
  std::string serialNumber{};
  TypeSpecification typeSpecification{};
  PhysicalParameters physicalParameters{};
  ProtocolLimits protocolLimits{};
  ProtocolFeatures protocolFeatures{};
  MobileRobotGeometry mobileRobotGeometry{};
  LoadSpecification loadSpecification{};
  std::optional<MobileRobotConfiguration> mobileRobotConfiguration{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    Factsheet, headerId, timestamp, version, manufacturer, serialNumber,
    typeSpecification, physicalParameters, protocolLimits, protocolFeatures,
    mobileRobotGeometry, loadSpecification, mobileRobotConfiguration);

} // namespace syrius_orbit::vda5050
