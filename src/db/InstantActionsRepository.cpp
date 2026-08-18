#include "syrius_orbit/db/InstantActionsRepository.hpp"

namespace syrius_orbit {

InstantActionsRepository::InstantActionsRepository(Database& db)
    : db_(db.handle()) {}

std::optional<InstantAction> InstantActionsRepository::findById(
    const std::string& action_id) {
  SQLite::Statement stmt(
      db_,
      "SELECT action_id, action_type, action_descriptor, blocking_type, "
      "       action_parameters, retriable, action_status, action_result, "
      "       robot_id, site_id, archived, created_at, last_updated_at "
      "FROM instant_actions WHERE action_id = ?");
  stmt.bind(1, action_id);

  if (!stmt.executeStep())
    return std::nullopt;

  return fromRow(stmt);
}

std::vector<InstantAction> InstantActionsRepository::listAll() {
  SQLite::Statement stmt(
      db_,
      "SELECT action_id, action_type, action_descriptor, blocking_type, "
      "       action_parameters, retriable, action_status, action_result, "
      "       robot_id, site_id, archived, created_at, last_updated_at "
      "FROM instant_actions");

  std::vector<InstantAction> results;
  while (stmt.executeStep())
    results.push_back(fromRow(stmt));
  return results;
}

void InstantActionsRepository::upsert(const InstantAction& action) {
  SQLite::Statement stmt(
      db_,
      "INSERT INTO instant_actions "
      "(action_id, action_type, action_descriptor, blocking_type, "
      " action_parameters, retriable, action_status, action_result, "
      " robot_id, site_id, archived, created_at, last_updated_at) "
      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
      "ON CONFLICT(action_id) DO UPDATE SET "
      "  action_type         = excluded.action_type, "
      "  action_descriptor   = excluded.action_descriptor, "
      "  blocking_type       = excluded.blocking_type, "
      "  action_parameters   = excluded.action_parameters, "
      "  retriable           = excluded.retriable, "
      "  action_status       = excluded.action_status, "
      "  action_result       = excluded.action_result, "
      "  robot_id            = excluded.robot_id, "
      "  site_id             = excluded.site_id, "
      "  archived            = excluded.archived, "
      "  last_updated_at     = excluded.last_updated_at");

  stmt.bind(1, action.action_id);
  stmt.bind(2, action.action_type);
  if (action.action_descriptor.has_value())
    stmt.bind(3, *action.action_descriptor);
  else
    stmt.bind(3);
  stmt.bind(4, action.blocking_type);
  if (action.action_parameters.has_value())
    stmt.bind(5, *action.action_parameters);
  else
    stmt.bind(5);
  stmt.bind(6, action.retriable ? 1 : 0);
  stmt.bind(7, action.action_status);
  if (action.action_result.has_value())
    stmt.bind(8, *action.action_result);
  else
    stmt.bind(8);
  stmt.bind(9, action.robot_id);
  if (action.site_id.has_value())
    stmt.bind(10, *action.site_id);
  else
    stmt.bind(10);
  stmt.bind(11, action.archived ? 1 : 0);
  stmt.bind(12, action.created_at);
  stmt.bind(13, action.last_updated_at);

  stmt.exec();
}

InstantAction InstantActionsRepository::fromRow(const SQLite::Statement& stmt) {
  InstantAction action;
  action.action_id = stmt.getColumn(0).getString();
  action.action_type = stmt.getColumn(1).getString();
  if (!stmt.getColumn(2).isNull())
    action.action_descriptor = stmt.getColumn(2).getString();
  action.blocking_type = stmt.getColumn(3).getString();
  if (!stmt.getColumn(4).isNull())
    action.action_parameters = stmt.getColumn(4).getString();
  action.retriable = stmt.getColumn(5).getInt() != 0;
  action.action_status = stmt.getColumn(6).getString();
  if (!stmt.getColumn(7).isNull())
    action.action_result = stmt.getColumn(7).getString();
  action.robot_id = stmt.getColumn(8).getString();
  if (!stmt.getColumn(9).isNull())
    action.site_id = stmt.getColumn(9).getString();
  action.archived = stmt.getColumn(10).getInt() != 0;
  action.created_at = stmt.getColumn(11).getString();
  action.last_updated_at = stmt.getColumn(12).getString();
  return action;
}

}  // namespace syrius_orbit