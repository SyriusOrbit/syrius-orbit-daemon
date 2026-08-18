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

}  // namespace syrius_orbit