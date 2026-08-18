#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>

#include "syrius_orbit/db/Database.hpp"

namespace syrius_orbit {

/// Represents a map record in the read model.
struct Map {
  std::string map_id;
  std::string map_version;
  std::optional<std::string> map_descriptor;
  std::string map_status = "ENABLED";
  std::string site_id;
  std::optional<std::string> graph_source;
  std::optional<std::string> zone_set_source;
  std::string last_updated_at;
};

/// Repository for the maps read table.
class MapsRepository {
 public:
  explicit MapsRepository(Database& db);

  /// Finds a map by its composite key (map_id, site_id).
  /// Returns nullopt if not found.
  std::optional<Map> findById(const std::string& map_id,
                              const std::string& site_id);

  /// Lists all maps.
  std::vector<Map> listAll();

  /// Inserts or updates a map record (for projection layer).
  void upsert(const Map& map);

 private:
  static Map fromRow(const SQLite::Statement& stmt);
  SQLite::Database& db_;
};

}  // namespace syrius_orbit