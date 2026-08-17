#include "syrius_orbit/SchemaMigrator.hpp"

#include <set>
#include <string>

#include <plog/Log.h>

#include "syrius_orbit/migrations_generated.h"

namespace syrius_orbit {

SchemaMigrator::SchemaMigrator(const std::string& db_path)
    : db_(db_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
  bootstrap();
}

SchemaMigrator::~SchemaMigrator() = default;

int SchemaMigrator::migrate() {
  // Collect already-applied versions.
  std::set<std::string> applied;
  SQLite::Statement query(db_, "SELECT version FROM schema_migrations");
  while (query.executeStep())
    applied.insert(query.getColumn(0).getString());

  int count = 0;
  for (const auto& migration : EMBEDDED_MIGRATIONS) {
    if (applied.contains(migration.version)) continue;

    PLOGI << "Applying migration " << migration.version << " " << migration.name;

    try {
      SQLite::Transaction txn(db_);
      db_.execMulti(std::string(migration.sql));

      SQLite::Statement insert(
          db_,
          "INSERT INTO schema_migrations (version, name, executed_at) "
          "VALUES (?, ?, datetime('now'))");
      insert.bind(1, std::string(migration.version));
      insert.bind(2, std::string(migration.name));
      insert.exec();
      txn.commit();
      ++count;
    } catch (const std::exception& e) {
      PLOGE << "Migration " << migration.version << " " << migration.name
            << " failed: " << e.what();
      throw;
    }
  }

  if (count > 0)
    PLOGI << "Schema migration complete: applied " << count << " migration(s)";
  else
    PLOGD << "Schema migration: database is up to date";

  return count;
}

void SchemaMigrator::bootstrap() {
  db_.exec(
      "CREATE TABLE IF NOT EXISTS schema_migrations ("
      "  version       TEXT PRIMARY KEY,"
      "  name          TEXT NOT NULL,"
      "  executed_at   TEXT NOT NULL"
      ")");
}

}  // namespace syrius_orbit
