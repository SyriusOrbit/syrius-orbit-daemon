#pragma once

#include <string_view>

#include <SQLiteCpp/SQLiteCpp.h>

namespace syrius_orbit {

struct Migration {
  std::string_view version;
  std::string_view name;
  std::string_view sql;
};

/// Applies embedded SQL migrations to a SQLite database.
class SchemaMigrator {
 public:
  explicit SchemaMigrator(const std::string& db_path);
  ~SchemaMigrator();

  SchemaMigrator(const SchemaMigrator&) = delete;
  SchemaMigrator& operator=(const SchemaMigrator&) = delete;

  /// Executes pending migrations in version order.
  /// Returns the number of migrations applied. Throws on failure.
  int migrate();

 private:
  void bootstrap();

  SQLite::Database db_;
};

}  // namespace syrius_orbit
