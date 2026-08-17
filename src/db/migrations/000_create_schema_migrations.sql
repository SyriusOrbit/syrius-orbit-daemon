-- Migration 000: Create schema_migrations table.
--
-- Tracks which migration files have been applied to the database.
-- The SchemaMigrator reads this table before executing pending
-- migrations and inserts a row after each successful execution.
--
-- version is the zero-padded migration number (e.g. "001", "002").
-- name is the descriptive part of the filename (e.g. "create_robots").
-- executed_at is an ISO-8601 timestamp recording when the migration ran.

CREATE TABLE schema_migrations (
    version       TEXT PRIMARY KEY,                  -- e.g. "001", "002"
    name          TEXT NOT NULL,                     -- e.g. "create_robots"
    executed_at   TEXT NOT NULL                      -- ISO-8601
);
