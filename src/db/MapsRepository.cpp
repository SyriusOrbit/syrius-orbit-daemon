#include "syrius_orbit/db/MapsRepository.hpp"

namespace syrius_orbit {

MapsRepository::MapsRepository(Database& db)
    : db_(db.handle()) {}

std::optional<Map> MapsRepository::findById(const std::string& map_id,
                                            const std::string& site_id) {
  SQLite::Statement stmt(
      db_,
      "SELECT map_id, map_version, map_descriptor, map_status, site_id, "
      "       graph_source, zone_set_source, last_updated_at "
      "FROM maps WHERE map_id = ? AND site_id = ?");
  stmt.bind(1, map_id);
  stmt.bind(2, site_id);

  if (!stmt.executeStep())
    return std::nullopt;

  return fromRow(stmt);
}

std::vector<Map> MapsRepository::listAll() {
  SQLite::Statement stmt(
      db_,
      "SELECT map_id, map_version, map_descriptor, map_status, site_id, "
      "       graph_source, zone_set_source, last_updated_at "
      "FROM maps");

  std::vector<Map> results;
  while (stmt.executeStep())
    results.push_back(fromRow(stmt));
  return results;
}

void MapsRepository::upsert(const Map& map) {
  SQLite::Statement stmt(
      db_,
      "INSERT INTO maps "
      "(map_id, map_version, map_descriptor, map_status, site_id, "
      " graph_source, zone_set_source, last_updated_at) "
      "VALUES (?, ?, ?, ?, ?, ?, ?, ?) "
      "ON CONFLICT(map_id, site_id) DO UPDATE SET "
      "  map_version       = excluded.map_version, "
      "  map_descriptor    = excluded.map_descriptor, "
      "  map_status        = excluded.map_status, "
      "  graph_source      = excluded.graph_source, "
      "  zone_set_source   = excluded.zone_set_source, "
      "  last_updated_at   = excluded.last_updated_at");

  stmt.bind(1, map.map_id);
  stmt.bind(2, map.map_version);
  if (map.map_descriptor.has_value())
    stmt.bind(3, *map.map_descriptor);
  else
    stmt.bind(3);
  stmt.bind(4, map.map_status);
  stmt.bind(5, map.site_id);
  if (map.graph_source.has_value())
    stmt.bind(6, *map.graph_source);
  else
    stmt.bind(6);
  if (map.zone_set_source.has_value())
    stmt.bind(7, *map.zone_set_source);
  else
    stmt.bind(7);
  stmt.bind(8, map.last_updated_at);

  stmt.exec();
}

Map MapsRepository::fromRow(const SQLite::Statement& stmt) {
  Map map;
  map.map_id = stmt.getColumn(0).getString();
  map.map_version = stmt.getColumn(1).getString();
  if (!stmt.getColumn(2).isNull())
    map.map_descriptor = stmt.getColumn(2).getString();
  map.map_status = stmt.getColumn(3).getString();
  map.site_id = stmt.getColumn(4).getString();
  if (!stmt.getColumn(5).isNull())
    map.graph_source = stmt.getColumn(5).getString();
  if (!stmt.getColumn(6).isNull())
    map.zone_set_source = stmt.getColumn(6).getString();
  map.last_updated_at = stmt.getColumn(7).getString();
  return map;
}

}  // namespace syrius_orbit