#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050::schema::factsheet {

struct FactsheetMessageTypeSpecification {
  std::string seriesName{};
  std::optional<std::string> seriesDescription{};
  std::string mobileRobotKinematics{};
  std::string mobileRobotClass{};
  double maximumLoadMass{};
  std::vector<std::string> localizationTypes{};
  std::vector<std::string> navigationTypes{};
  std::optional<std::vector<std::string>> supportedZones{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageTypeSpecification, seriesName, seriesDescription, mobileRobotKinematics, mobileRobotClass, maximumLoadMass, localizationTypes, navigationTypes, supportedZones);

struct FactsheetMessagePhysicalParameters {
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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessagePhysicalParameters, minimumSpeed, maximumSpeed, minimumAngularSpeed, maximumAngularSpeed, maximumAcceleration, maximumDeceleration, minimumHeight, maximumHeight, width, length);

struct FactsheetMessageProtocolLimitsMaximumStringLengths {
  std::optional<std::int64_t> maximumMessageLength{};
  std::optional<std::int64_t> maximumTopicSerialLength{};
  std::optional<std::int64_t> maximumTopicElementLength{};
  std::optional<std::int64_t> maximumIdLength{};
  std::optional<bool> idNumericalOnly{};
  std::optional<std::int64_t> maximumLoadIdLength{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageProtocolLimitsMaximumStringLengths, maximumMessageLength, maximumTopicSerialLength, maximumTopicElementLength, maximumIdLength, idNumericalOnly, maximumLoadIdLength);

struct FactsheetMessageProtocolLimitsMaximumArrayLengths {
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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageProtocolLimitsMaximumArrayLengths, order_nodes, order_edges, node_actions, edge_actions, actions_actionsParameters, instantActions, trajectory_knotVector, trajectory_controlPoints, zoneSet_zones, state_nodeStates, state_edgeStates, state_loads, state_actionStates, state_instantActionStates, state_zoneActionStates, state_errors, state_information, error_errorReferences, information_infoReferences);

struct FactsheetMessageProtocolLimitsTiming {
  double minimumOrderInterval{};
  double minimumStateInterval{};
  std::optional<double> defaultStateInterval{};
  std::optional<double> visualizationInterval{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageProtocolLimitsTiming, minimumOrderInterval, minimumStateInterval, defaultStateInterval, visualizationInterval);

struct FactsheetMessageProtocolLimits {
  FactsheetMessageProtocolLimitsMaximumStringLengths maximumStringLengths{};
  FactsheetMessageProtocolLimitsMaximumArrayLengths maximumArrayLengths{};
  FactsheetMessageProtocolLimitsTiming timing{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageProtocolLimits, maximumStringLengths, maximumArrayLengths, timing);

struct FactsheetMessageProtocolFeaturesOptionalParametersItem {
  std::string parameter{};
  std::string support{};
  std::optional<std::string> description{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageProtocolFeaturesOptionalParametersItem, parameter, support, description);

struct FactsheetMessageProtocolFeaturesMobileRobotActionsItemActionParametersItem {
  std::string key{};
  std::string valueDataType{};
  std::optional<std::string> description{};
  std::optional<bool> isOptional{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageProtocolFeaturesMobileRobotActionsItemActionParametersItem, key, valueDataType, description, isOptional);

struct FactsheetMessageProtocolFeaturesMobileRobotActionsItem {
  std::string actionType{};
  std::optional<std::string> actionDescription{};
  std::vector<std::string> actionScopes{};
  std::optional<std::vector<FactsheetMessageProtocolFeaturesMobileRobotActionsItemActionParametersItem>> actionParameters{};
  std::optional<std::string> actionResult{};
  std::optional<std::vector<std::string>> blockingTypes{};
  bool pauseAllowed{};
  bool cancelAllowed{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageProtocolFeaturesMobileRobotActionsItem, actionType, actionDescription, actionScopes, actionParameters, actionResult, blockingTypes, pauseAllowed, cancelAllowed);

struct FactsheetMessageProtocolFeatures {
  std::vector<FactsheetMessageProtocolFeaturesOptionalParametersItem> optionalParameters{};
  std::vector<FactsheetMessageProtocolFeaturesMobileRobotActionsItem> mobileRobotActions{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageProtocolFeatures, optionalParameters, mobileRobotActions);

struct FactsheetMessageMobileRobotGeometryWheelDefinitionsItemPosition {
  double x{};
  double y{};
  std::optional<double> theta{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageMobileRobotGeometryWheelDefinitionsItemPosition, x, y, theta);

struct FactsheetMessageMobileRobotGeometryWheelDefinitionsItem {
  std::string type{};
  bool isActiveDriven{};
  bool isActiveSteered{};
  FactsheetMessageMobileRobotGeometryWheelDefinitionsItemPosition position{};
  double diameter{};
  double width{};
  std::optional<double> centerDisplacement{};
  std::optional<std::string> constraints{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageMobileRobotGeometryWheelDefinitionsItem, type, isActiveDriven, isActiveSteered, position, diameter, width, centerDisplacement, constraints);

struct FactsheetMessageMobileRobotGeometryEnvelopes2dItemVerticesItem {
  double x{};
  double y{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageMobileRobotGeometryEnvelopes2dItemVerticesItem, x, y);

struct FactsheetMessageMobileRobotGeometryEnvelopes2dItem {
  std::string envelope2dId{};
  std::vector<FactsheetMessageMobileRobotGeometryEnvelopes2dItemVerticesItem> vertices{};
  std::optional<std::string> description{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageMobileRobotGeometryEnvelopes2dItem, envelope2dId, vertices, description);

struct FactsheetMessageMobileRobotGeometryEnvelopes3dItem {
  std::string envelope3dId{};
  std::string format{};
  std::optional<nlohmann::json> data{};
  std::optional<std::string> url{};
  std::optional<std::string> description{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageMobileRobotGeometryEnvelopes3dItem, envelope3dId, format, data, url, description);

struct FactsheetMessageMobileRobotGeometry {
  std::optional<std::vector<FactsheetMessageMobileRobotGeometryWheelDefinitionsItem>> wheelDefinitions{};
  std::optional<std::vector<FactsheetMessageMobileRobotGeometryEnvelopes2dItem>> envelopes2d{};
  std::optional<std::vector<FactsheetMessageMobileRobotGeometryEnvelopes3dItem>> envelopes3d{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageMobileRobotGeometry, wheelDefinitions, envelopes2d, envelopes3d);

struct FactsheetMessageLoadSpecificationLoadSetsItemBoundingBoxReference {
  double x{};
  double y{};
  double z{};
  std::optional<double> theta{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageLoadSpecificationLoadSetsItemBoundingBoxReference, x, y, z, theta);

struct FactsheetMessageLoadSpecificationLoadSetsItemLoadDimensions {
  double length{};
  double width{};
  std::optional<double> height{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageLoadSpecificationLoadSetsItemLoadDimensions, length, width, height);

struct FactsheetMessageLoadSpecificationLoadSetsItem {
  std::string setName{};
  std::string loadType{};
  std::optional<std::vector<std::string>> loadPositions{};
  std::optional<FactsheetMessageLoadSpecificationLoadSetsItemBoundingBoxReference> boundingBoxReference{};
  std::optional<FactsheetMessageLoadSpecificationLoadSetsItemLoadDimensions> loadDimensions{};
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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageLoadSpecificationLoadSetsItem, setName, loadType, loadPositions, boundingBoxReference, loadDimensions, maximumWeight, minimumLoadhandlingHeight, maximumLoadhandlingHeight, minimumLoadhandlingDepth, maximumLoadhandlingDepth, minimumLoadhandlingTilt, maximumLoadhandlingTilt, maximumSpeed, maximumAcceleration, maximumDeceleration, pickTime, dropTime, description);

struct FactsheetMessageLoadSpecification {
  std::optional<std::vector<std::string>> loadPositions{};
  std::optional<std::vector<FactsheetMessageLoadSpecificationLoadSetsItem>> loadSets{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageLoadSpecification, loadPositions, loadSets);

struct FactsheetMessageMobileRobotConfigurationVersionsItem {
  std::string key{};
  std::string value{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageMobileRobotConfigurationVersionsItem, key, value);

struct FactsheetMessageMobileRobotConfigurationNetwork {
  std::optional<std::vector<std::string>> dnsServers{};
  std::optional<std::vector<std::string>> ntpServers{};
  std::optional<std::string> localIpAddress{};
  std::optional<std::string> netmask{};
  std::optional<std::string> defaultGateway{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageMobileRobotConfigurationNetwork, dnsServers, ntpServers, localIpAddress, netmask, defaultGateway);

struct FactsheetMessageMobileRobotConfigurationBatteryCharging {
  std::optional<double> criticalLowChargingLevel{};
  std::optional<double> minimumDesiredChargingLevel{};
  std::optional<double> maximumDesiredChargingLevel{};
  std::optional<double> minimumChargingTime{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageMobileRobotConfigurationBatteryCharging, criticalLowChargingLevel, minimumDesiredChargingLevel, maximumDesiredChargingLevel, minimumChargingTime);

struct FactsheetMessageMobileRobotConfiguration {
  std::optional<std::vector<FactsheetMessageMobileRobotConfigurationVersionsItem>> versions{};
  std::optional<FactsheetMessageMobileRobotConfigurationNetwork> network{};
  std::optional<FactsheetMessageMobileRobotConfigurationBatteryCharging> batteryCharging{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessageMobileRobotConfiguration, versions, network, batteryCharging);

struct FactsheetMessage {
  std::int64_t headerId{};
  std::string timestamp{};
  std::string version{};
  std::string manufacturer{};
  std::string serialNumber{};
  FactsheetMessageTypeSpecification typeSpecification{};
  FactsheetMessagePhysicalParameters physicalParameters{};
  FactsheetMessageProtocolLimits protocolLimits{};
  FactsheetMessageProtocolFeatures protocolFeatures{};
  FactsheetMessageMobileRobotGeometry mobileRobotGeometry{};
  FactsheetMessageLoadSpecification loadSpecification{};
  std::optional<FactsheetMessageMobileRobotConfiguration> mobileRobotConfiguration{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FactsheetMessage, headerId, timestamp, version, manufacturer, serialNumber, typeSpecification, physicalParameters, protocolLimits, protocolFeatures, mobileRobotGeometry, loadSpecification, mobileRobotConfiguration);

} // namespace syrius_orbit::vda5050::schema::factsheet
