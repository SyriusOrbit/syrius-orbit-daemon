#include "syrius_orbit/db/Database.hpp"

#include "syrius_orbit/SchemaMigrator.hpp"

namespace syrius_orbit {

Database::Database(const std::string& db_path)
    : db_path_(db_path),
      db_(db_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {}

Database::~Database() = default;

void Database::migrate() {
  SchemaMigrator migrator(db_path_);
  migrator.migrate();
}

}  // namespace syrius_orbit