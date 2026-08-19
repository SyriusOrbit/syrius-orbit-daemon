#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>

#include "syrius_orbit/db/Database.hpp"

namespace syrius_orbit {

/// Represents a robot record in the read model.
struct Robot {
  std::string robot_id;
  std::string manufacturer;
  std::string serial_number;
  std::optional<std::string> site_id;
  std::optional<std::string> display_name;
  std::string connection_state = "OFFLINE";
  std::optional<std::string> operating_mode;
  bool driving = false;
  bool paused = false;
  std::optional<double> battery_level;
  bool charging = false;
  bool suspended = false;
  bool archived = false;
  std::optional<std::string> factsheet;  // JSON
  std::string last_updated_at;
  std::string created_at;
};

/// Repository for the robots read table.
class RobotsRepository {
 public:
  explicit RobotsRepository(Database& db);

  /// Finds a robot by its robot_id. Returns nullopt if not found.
  std::optional<Robot> findById(const std::string& robot_id);

  /// Lists all robots.
  std::vector<Robot> listAll();

  /// Inserts or updates a robot record (for projection layer).
  void upsert(const Robot& robot);

  /// Updates connection-related columns only (projection: connection topic).
  void upsertConnection(const std::string& robot_id,
                        const std::string& manufacturer,
                        const std::string& serial_number,
                        const std::string& connection_state,
                        const std::string& last_updated_at);

  /// Updates state-related columns only (projection: state topic).
  void upsertState(const std::string& robot_id,
                   const std::string& manufacturer,
                   const std::string& serial_number,
                   const std::string& operating_mode,
                   bool driving,
                   bool paused,
                   double battery_level,
                   bool charging,
                   const std::string& last_updated_at);

  /// Updates factsheet column only (projection: factsheet topic).
  void upsertFactsheet(const std::string& robot_id,
                       const std::string& manufacturer,
                       const std::string& serial_number,
                       const std::string& factsheet,
                       const std::string& last_updated_at);

 private:
  static Robot fromRow(const SQLite::Statement& stmt);
  SQLite::Database& db_;
};

}  // namespace syrius_orbit