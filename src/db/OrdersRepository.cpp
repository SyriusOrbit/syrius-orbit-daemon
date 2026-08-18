#include "syrius_orbit/db/OrdersRepository.hpp"

namespace syrius_orbit {

OrdersRepository::OrdersRepository(Database& db)
    : db_(db.handle()) {}

std::optional<Order> OrdersRepository::findById(const std::string& order_id) {
  SQLite::Statement stmt(
      db_,
      "SELECT order_id, order_update_id, order_description, status, "
      "       assigned_robot_id, site_id, nodes, edges, archived, "
      "       created_at, last_updated_at "
      "FROM orders WHERE order_id = ?");
  stmt.bind(1, order_id);

  if (!stmt.executeStep())
    return std::nullopt;

  return fromRow(stmt);
}

std::vector<Order> OrdersRepository::listAll() {
  SQLite::Statement stmt(
      db_,
      "SELECT order_id, order_update_id, order_description, status, "
      "       assigned_robot_id, site_id, nodes, edges, archived, "
      "       created_at, last_updated_at "
      "FROM orders");

  std::vector<Order> results;
  while (stmt.executeStep())
    results.push_back(fromRow(stmt));
  return results;
}

void OrdersRepository::upsert(const Order& order) {
  SQLite::Statement stmt(
      db_,
      "INSERT INTO orders "
      "(order_id, order_update_id, order_description, status, "
      " assigned_robot_id, site_id, nodes, edges, archived, "
      " created_at, last_updated_at) "
      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
      "ON CONFLICT(order_id) DO UPDATE SET "
      "  order_update_id    = excluded.order_update_id, "
      "  order_description  = excluded.order_description, "
      "  status             = excluded.status, "
      "  assigned_robot_id  = excluded.assigned_robot_id, "
      "  site_id            = excluded.site_id, "
      "  nodes              = excluded.nodes, "
      "  edges              = excluded.edges, "
      "  archived           = excluded.archived, "
      "  last_updated_at    = excluded.last_updated_at");

  stmt.bind(1, order.order_id);
  stmt.bind(2, order.order_update_id);
  if (order.order_description.has_value())
    stmt.bind(3, *order.order_description);
  else
    stmt.bind(3);
  stmt.bind(4, order.status);
  stmt.bind(5, order.assigned_robot_id);
  stmt.bind(6, order.site_id);
  stmt.bind(7, order.nodes);
  stmt.bind(8, order.edges);
  stmt.bind(9, order.archived ? 1 : 0);
  stmt.bind(10, order.created_at);
  stmt.bind(11, order.last_updated_at);

  stmt.exec();
}

Order OrdersRepository::fromRow(const SQLite::Statement& stmt) {
  Order order;
  order.order_id = stmt.getColumn(0).getString();
  order.order_update_id = stmt.getColumn(1).getInt64();
  if (!stmt.getColumn(2).isNull())
    order.order_description = stmt.getColumn(2).getString();
  order.status = stmt.getColumn(3).getString();
  order.assigned_robot_id = stmt.getColumn(4).getString();
  order.site_id = stmt.getColumn(5).getString();
  order.nodes = stmt.getColumn(6).getString();
  order.edges = stmt.getColumn(7).getString();
  order.archived = stmt.getColumn(8).getInt() != 0;
  order.created_at = stmt.getColumn(9).getString();
  order.last_updated_at = stmt.getColumn(10).getString();
  return order;
}

}  // namespace syrius_orbit