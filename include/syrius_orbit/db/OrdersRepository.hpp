#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>

#include "syrius_orbit/db/Database.hpp"

namespace syrius_orbit {

/// Represents an order record in the read model.
struct Order {
  std::string order_id;
  int64_t order_update_id = 0;
  std::optional<std::string> order_description;
  std::string status = "QUEUED";
  std::string assigned_robot_id;
  std::string site_id;
  std::string nodes;   // JSON
  std::string edges;   // JSON
  bool archived = false;
  std::string created_at;
  std::string last_updated_at;
};

/// Repository for the orders read table.
class OrdersRepository {
 public:
  explicit OrdersRepository(Database& db);

  /// Finds an order by its order_id. Returns nullopt if not found.
  std::optional<Order> findById(const std::string& order_id);

  /// Lists all orders.
  std::vector<Order> listAll();

  /// Inserts or updates an order record (for projection layer).
  void upsert(const Order& order);

 private:
  static Order fromRow(const SQLite::Statement& stmt);
  SQLite::Database& db_;
};

}  // namespace syrius_orbit