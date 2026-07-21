#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace syrius_orbit::vda5050 {

enum class ConnectionState {
  ONLINE,
  OFFLINE,
  HIBERNATING,
  CONNECTION_BROKEN,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ConnectionState,
                             {
                                 {ConnectionState::ONLINE, "ONLINE"},
                                 {ConnectionState::OFFLINE, "OFFLINE"},
                                 {ConnectionState::HIBERNATING, "HIBERNATING"},
                                 {ConnectionState::CONNECTION_BROKEN,
                                  "CONNECTION_BROKEN"},
                             });

struct Connection {
  std::int64_t headerId{};
  std::string timestamp{};
  std::string version{};
  std::string manufacturer{};
  std::string serialNumber{};
  ConnectionState connectionState{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Connection, headerId, timestamp,
                                                version, manufacturer,
                                                serialNumber, connectionState);

} // namespace syrius_orbit::vda5050
