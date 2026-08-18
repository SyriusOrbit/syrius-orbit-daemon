#pragma once

#include <string>

#include <SQLiteCpp/SQLiteCpp.h>

namespace syrius_orbit {

/// Manages the SQLite database connection and lifecycle.
///
/// Opens the database on construction and provides access to the underlying
/// SQLite::Database handle for repositories. The migrate() method must be
/// called once before any queries are executed.
class Database {
 public:
  explicit Database(const std::string& db_path);
  ~Database();

  Database(const Database&) = delete;
  Database& operator=(const Database&) = delete;
  Database(Database&&) = delete;
  Database& operator=(Database&&) = delete;

  /// Runs pending SQL migrations. Must be called before using repositories.
  void migrate();

  /// Returns the underlying SQLite database handle.
  SQLite::Database& handle() { return db_; }

 private:
  std::string db_path_;
  SQLite::Database db_;
};

}  // namespace syrius_orbit