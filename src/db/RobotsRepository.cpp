#include "syrius_orbit/db/RobotsRepository.hpp"

#include <plog/Log.h>

namespace syrius_orbit {

RobotsRepository::RobotsRepository(Database& db)
    : db_(db.handle()) {}

std::optional<Robot> RobotsRepository::findById(const std::string& robot_id) {
  SQLite::Statement stmt(
      db_,
      "SELECT robot_id, manufacturer, serial_number, site_id, display_name, "
      "       connection_state, operating_mode, driving, paused, battery_level, "
      "       charging, suspended, archived, factsheet, last_updated_at, created_at "
      "FROM robots WHERE robot_id = ?");
  stmt.bind(1, robot_id);

  if (!stmt.executeStep())
    return std::nullopt;

  return fromRow(stmt);
}

std::vector<Robot> RobotsRepository::listAll() {
  SQLite::Statement stmt(
      db_,
      "SELECT robot_id, manufacturer, serial_number, site_id, display_name, "
      "       connection_state, operating_mode, driving, paused, battery_level, "
      "       charging, suspended, archived, factsheet, last_updated_at, created_at "
      "FROM robots");

  std::vector<Robot> results;
  while (stmt.executeStep())
    results.push_back(fromRow(stmt));
  return results;
}

void RobotsRepository::upsert(const Robot& robot) {
  SQLite::Statement stmt(
      db_,
      "INSERT INTO robots "
      "(robot_id, manufacturer, serial_number, site_id, display_name, "
      " connection_state, operating_mode, driving, paused, battery_level, "
      " charging, suspended, archived, factsheet, last_updated_at, created_at) "
      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
      "ON CONFLICT(robot_id) DO UPDATE SET "
      "  manufacturer      = excluded.manufacturer, "
      "  serial_number     = excluded.serial_number, "
      "  site_id           = excluded.site_id, "
      "  display_name      = excluded.display_name, "
      "  connection_state  = excluded.connection_state, "
      "  operating_mode    = excluded.operating_mode, "
      "  driving           = excluded.driving, "
      "  paused            = excluded.paused, "
      "  battery_level     = excluded.battery_level, "
      "  charging          = excluded.charging, "
      "  suspended         = excluded.suspended, "
      "  archived          = excluded.archived, "
      "  factsheet         = excluded.factsheet, "
      "  last_updated_at   = excluded.last_updated_at");

  stmt.bind(1, robot.robot_id);
  stmt.bind(2, robot.manufacturer);
  stmt.bind(3, robot.serial_number);
  if (robot.site_id.has_value())
    stmt.bind(4, *robot.site_id);
  else
    stmt.bind(4);
  if (robot.display_name.has_value())
    stmt.bind(5, *robot.display_name);
  else
    stmt.bind(5);
  stmt.bind(6, robot.connection_state);
  if (robot.operating_mode.has_value())
    stmt.bind(7, *robot.operating_mode);
  else
    stmt.bind(7);
  stmt.bind(8, robot.driving ? 1 : 0);
  stmt.bind(9, robot.paused ? 1 : 0);
  if (robot.battery_level.has_value())
    stmt.bind(10, *robot.battery_level);
  else
    stmt.bind(10);
  stmt.bind(11, robot.charging ? 1 : 0);
  stmt.bind(12, robot.suspended ? 1 : 0);
  stmt.bind(13, robot.archived ? 1 : 0);
  if (robot.factsheet.has_value())
    stmt.bind(14, *robot.factsheet);
  else
    stmt.bind(14);
  stmt.bind(15, robot.last_updated_at);
  stmt.bind(16, robot.created_at);

  stmt.exec();
}

void RobotsRepository::upsertConnection(
    const std::string& robot_id,
    const std::string& manufacturer,
    const std::string& serial_number,
    const std::string& connection_state,
    const std::string& last_updated_at) {
  SQLite::Statement stmt(
      db_,
      "INSERT INTO robots "
      "(robot_id, manufacturer, serial_number, connection_state, "
      " last_updated_at, created_at) "
      "VALUES (?, ?, ?, ?, ?, ?) "
      "ON CONFLICT(robot_id) DO UPDATE SET "
      "  connection_state  = excluded.connection_state, "
      "  last_updated_at   = excluded.last_updated_at");

  stmt.bind(1, robot_id);
  stmt.bind(2, manufacturer);
  stmt.bind(3, serial_number);
  stmt.bind(4, connection_state);
  stmt.bind(5, last_updated_at);
  stmt.bind(6, last_updated_at);
  stmt.exec();
}

void RobotsRepository::upsertState(
    const std::string& robot_id,
    const std::string& manufacturer,
    const std::string& serial_number,
    const std::string& operating_mode,
    bool driving,
    bool paused,
    double battery_level,
    bool charging,
    const std::string& last_updated_at) {
  SQLite::Statement stmt(
      db_,
      "INSERT INTO robots "
      "(robot_id, manufacturer, serial_number, operating_mode, driving, "
      " paused, battery_level, charging, last_updated_at, created_at) "
      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
      "ON CONFLICT(robot_id) DO UPDATE SET "
      "  operating_mode  = excluded.operating_mode, "
      "  driving         = excluded.driving, "
      "  paused          = excluded.paused, "
      "  battery_level   = excluded.battery_level, "
      "  charging        = excluded.charging, "
      "  last_updated_at = excluded.last_updated_at");

  stmt.bind(1, robot_id);
  stmt.bind(2, manufacturer);
  stmt.bind(3, serial_number);
  stmt.bind(4, operating_mode);
  stmt.bind(5, driving ? 1 : 0);
  stmt.bind(6, paused ? 1 : 0);
  stmt.bind(7, battery_level);
  stmt.bind(8, charging ? 1 : 0);
  stmt.bind(9, last_updated_at);
  stmt.bind(10, last_updated_at);
  stmt.exec();
}

void RobotsRepository::upsertFactsheet(
    const std::string& robot_id,
    const std::string& manufacturer,
    const std::string& serial_number,
    const std::string& factsheet,
    const std::string& last_updated_at) {
  SQLite::Statement stmt(
      db_,
      "INSERT INTO robots "
      "(robot_id, manufacturer, serial_number, factsheet, "
      " last_updated_at, created_at) "
      "VALUES (?, ?, ?, ?, ?, ?) "
      "ON CONFLICT(robot_id) DO UPDATE SET "
      "  factsheet        = excluded.factsheet, "
      "  last_updated_at  = excluded.last_updated_at");

  stmt.bind(1, robot_id);
  stmt.bind(2, manufacturer);
  stmt.bind(3, serial_number);
  stmt.bind(4, factsheet);
  stmt.bind(5, last_updated_at);
  stmt.bind(6, last_updated_at);
  stmt.exec();
}

Robot RobotsRepository::fromRow(const SQLite::Statement& stmt) {
  Robot robot;
  robot.robot_id = stmt.getColumn(0).getString();
  robot.manufacturer = stmt.getColumn(1).getString();
  robot.serial_number = stmt.getColumn(2).getString();
  if (!stmt.getColumn(3).isNull())
    robot.site_id = stmt.getColumn(3).getString();
  if (!stmt.getColumn(4).isNull())
    robot.display_name = stmt.getColumn(4).getString();
  robot.connection_state = stmt.getColumn(5).getString();
  if (!stmt.getColumn(6).isNull())
    robot.operating_mode = stmt.getColumn(6).getString();
  robot.driving = stmt.getColumn(7).getInt() != 0;
  robot.paused = stmt.getColumn(8).getInt() != 0;
  if (!stmt.getColumn(9).isNull())
    robot.battery_level = stmt.getColumn(9).getDouble();
  robot.charging = stmt.getColumn(10).getInt() != 0;
  robot.suspended = stmt.getColumn(11).getInt() != 0;
  robot.archived = stmt.getColumn(12).getInt() != 0;
  if (!stmt.getColumn(13).isNull())
    robot.factsheet = stmt.getColumn(13).getString();
  robot.last_updated_at = stmt.getColumn(14).getString();
  robot.created_at = stmt.getColumn(15).getString();
  return robot;
}

}  // namespace syrius_orbit