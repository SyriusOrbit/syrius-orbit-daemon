#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050::schema::responses {

struct ResponsesMessageResponsesItem {
  std::string requestId{};
  std::string grantType{};
  std::optional<std::string> leaseExpiry{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ResponsesMessageResponsesItem, requestId, grantType, leaseExpiry);

struct ResponsesMessage {
  std::int64_t headerId{};
  std::string timestamp{};
  std::string version{};
  std::string manufacturer{};
  std::string serialNumber{};
  std::vector<ResponsesMessageResponsesItem> responses{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ResponsesMessage, headerId, timestamp, version, manufacturer, serialNumber, responses);

} // namespace syrius_orbit::vda5050::schema::responses
