#pragma once

#include <mutex>
#include <string>

#include <SQLiteCpp/SQLiteCpp.h>

namespace syrius_orbit {

/// Manages the SQLite database connection and lifecycle.
///
/// Opens the database on construction and provides access to the underlying
/// SQLite::Database handle for repositories. The migrate() method must be
/// called once before any queries are executed.
///
/// Provides a mutex for serializing cross-thread database access (e.g.,
/// between the HTTP server and the projection engine).
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

  /// Locks the database mutex for cross-thread serialization.
  void lock() { mutex_.lock(); }

  /// Unlocks the database mutex.
  void unlock() { mutex_.unlock(); }

 private:
  std::string db_path_;
  SQLite::Database db_;
  std::mutex mutex_;
};

}  // namespace syrius_orbit