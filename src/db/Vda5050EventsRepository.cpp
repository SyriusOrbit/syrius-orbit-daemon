#include "syrius_orbit/db/Vda5050EventsRepository.hpp"

namespace syrius_orbit {

Vda5050EventsRepository::Vda5050EventsRepository(Database& db)
    : db_(db.handle()) {}

int64_t Vda5050EventsRepository::insert(const Vda5050Event& event) {
  SQLite::Statement stmt(
      db_,
      "INSERT INTO vda5050_events "
      "(topic, item_id, received_at, manufacturer, serial_number, robot_id, "
      " direction, header_id, header_version, header_timestamp, payload) "
      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

  stmt.bind(1, event.topic);
  stmt.bind(2, event.item_id);
  stmt.bind(3, event.received_at);
  stmt.bind(4, event.manufacturer);
  stmt.bind(5, event.serial_number);
  stmt.bind(6, event.robot_id);
  stmt.bind(7, event.direction);
  if (event.header_id.has_value())
    stmt.bind(8, *event.header_id);
  else
    stmt.bind(8);
  if (event.header_version.has_value())
    stmt.bind(9, *event.header_version);
  else
    stmt.bind(9);
  if (event.header_timestamp.has_value())
    stmt.bind(10, *event.header_timestamp);
  else
    stmt.bind(10);
  stmt.bind(11, event.payload);

  stmt.exec();
  return db_.getLastInsertRowid();
}

std::vector<Vda5050Event> Vda5050EventsRepository::findLatestByTopic(
    const std::string& topic) {
  SQLite::Statement stmt(
      db_,
      "SELECT id, topic, item_id, received_at, manufacturer, serial_number, "
      "       robot_id, direction, header_id, header_version, "
      "       header_timestamp, payload "
      "FROM vda5050_events "
      "WHERE topic = ? "
      "AND id IN (SELECT MAX(id) FROM vda5050_events "
      "           WHERE topic = ? GROUP BY item_id)");

  stmt.bind(1, topic);
  stmt.bind(2, topic);

  std::vector<Vda5050Event> results;
  while (stmt.executeStep())
    results.push_back(fromRow(stmt));
  return results;
}

Vda5050Event Vda5050EventsRepository::fromRow(const SQLite::Statement& stmt) {
  Vda5050Event event;
  event.id = stmt.getColumn(0).getInt64();
  event.topic = stmt.getColumn(1).getString();
  event.item_id = stmt.getColumn(2).getString();
  event.received_at = stmt.getColumn(3).getString();
  event.manufacturer = stmt.getColumn(4).getString();
  event.serial_number = stmt.getColumn(5).getString();
  event.robot_id = stmt.getColumn(6).getString();
  event.direction = stmt.getColumn(7).getString();
  if (!stmt.getColumn(8).isNull())
    event.header_id = stmt.getColumn(8).getInt64();
  if (!stmt.getColumn(9).isNull())
    event.header_version = stmt.getColumn(9).getString();
  if (!stmt.getColumn(10).isNull())
    event.header_timestamp = stmt.getColumn(10).getString();
  event.payload = stmt.getColumn(11).getString();
  return event;
}

}  // namespace syrius_orbit