#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>

#include "syrius_orbit/db/Database.hpp"

namespace syrius_orbit {

/// Represents an instant action record in the read model.
struct InstantAction {
  std::string action_id;
  std::string action_type;
  std::optional<std::string> action_descriptor;
  std::string blocking_type = "NONE";
  std::optional<std::string> action_parameters;  // JSON
  bool retriable = false;
  std::string action_status = "WAITING";
  std::optional<std::string> action_result;
  std::string robot_id;
  std::optional<std::string> site_id;
  bool archived = false;
  std::string created_at;
  std::string last_updated_at;
};

/// Repository for the instant_actions read table.
class InstantActionsRepository {
 public:
  explicit InstantActionsRepository(Database& db);

  /// Finds an instant action by its action_id. Returns nullopt if not found.
  std::optional<InstantAction> findById(const std::string& action_id);

  /// Lists all instant actions.
  std::vector<InstantAction> listAll();

  /// Inserts or updates an instant action record (for projection layer).
  void upsert(const InstantAction& action);

  /// Updates action fields from a VDA5050 instant action event (projection).
  void upsertFromAction(const std::string& action_id,
                        const std::string& action_type,
                        const std::string& action_descriptor,
                        const std::string& blocking_type,
                        const std::string& action_parameters,
                        const std::string& robot_id,
                        const std::string& last_updated_at);

  /// Updates response status from a VDA5050 response event (projection).
  void upsertFromResponse(const std::string& action_id,
                          const std::string& robot_id,
                          const std::string& action_status,
                          const std::string& last_updated_at);

 private:
  static InstantAction fromRow(const SQLite::Statement& stmt);
  SQLite::Database& db_;
};

}  // namespace syrius_orbit