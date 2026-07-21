#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace syrius_orbit::vda5050 {

enum class GrantType {
  GRANTED,
  QUEUED,
  REVOKED,
  REJECTED,
};

NLOHMANN_JSON_SERIALIZE_ENUM(GrantType, {
                                            {GrantType::GRANTED, "GRANTED"},
                                            {GrantType::QUEUED, "QUEUED"},
                                            {GrantType::REVOKED, "REVOKED"},
                                            {GrantType::REJECTED, "REJECTED"},
                                        });

struct Response {
  std::string requestId{};
  GrantType grantType{};
  std::optional<std::string> leaseExpiry{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Response, requestId, grantType,
                                                leaseExpiry);

struct Responses {
  std::int64_t headerId{};
  std::string timestamp{};
  std::string version{};
  std::string manufacturer{};
  std::string serialNumber{};
  std::vector<Response> responses{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Responses, headerId, timestamp,
                                                version, manufacturer,
                                                serialNumber, responses);

} // namespace syrius_orbit::vda5050
