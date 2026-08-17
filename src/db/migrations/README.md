# Database Schema

This document describes the SQLite schema for the SyriusOrbit daemon.
All tables are created via numbered migration files under this directory.

## Table Overview

| Migration | Table | Description |
|-----------|-------|-------------|
| 001 | `robots` | Robot inventory, live summary fields, and factsheet. |
| 002 | `orders` | VDA 5050 order management. |
| 003 | `instant_actions` | Instant action history. |
| 004 | `maps` | Site-scoped map metadata with Spatial API references. |

The `schema_migrations` table is created by `SchemaMigrator::bootstrap()`
before any migration files are processed; it has no migration file.

## Storage Strategy

- **Embedded JSON columns**: Deeply nested structures that are read as a whole
  (factsheet, order nodes/edges, action parameters) are stored as SQLite `JSON`
  columns. They are not split into child tables. Query individual fields via
  SQLite JSON1 functions (`json_extract`, `json_each`) when needed.

- **External references**: Map sub-resources (navigation graph, zone sets) are
  not stored locally. The `maps` table holds `graph_source` and
  `zone_set_source` text columns that reference Spatial API Collection IDs.
  The Spatial API is the single source of truth for that data.

- **Dimension simplification**: `site_id` is a plain string dimension used
  across tables. No separate `sites` table exists.

---

## Table Details

### schema_migrations (bootstrap)

Created by `SchemaMigrator::bootstrap()` before any migration files are
processed. Tracks which migrations have been applied.

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| `version` | TEXT | PRIMARY KEY | Migration number, e.g. `"001"`, `"002"`. |
| `name` | TEXT | NOT NULL | Descriptive name, e.g. `"create_robots"`. |
| `executed_at` | TEXT | NOT NULL | ISO-8601 execution timestamp. |

---

### robots (Migration 001)

Stores the robot inventory with high-frequency query fields projected from
VDA 5050 state messages. Complete VDA 5050 state is not persisted; it arrives
near real time via MQTT.

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| `robot_id` | TEXT | PRIMARY KEY | `{manufacturer}.{serialNumber}`. |
| `manufacturer` | TEXT | NOT NULL | VDA 5050 `header.manufacturer`. |
| `serial_number` | TEXT | NOT NULL | VDA 5050 `header.serialNumber`. |
| `site_id` | TEXT | DEFAULT NULL | Logical site assignment. NULL = orphan. |
| `display_name` | TEXT | DEFAULT NULL | Management-side display name. |
| `connection_state` | TEXT | NOT NULL DEFAULT `'OFFLINE'` | VDA 5050 `connectionState` enum. |
| `operating_mode` | TEXT | DEFAULT NULL | VDA 5050 `state.operatingMode`. |
| `driving` | INTEGER | NOT NULL DEFAULT 0 | 0/1. True if driving or rotating. |
| `paused` | INTEGER | NOT NULL DEFAULT 0 | 0/1. |
| `battery_level` | REAL | DEFAULT NULL | `state.powerSupply.stateOfCharge` (0-100%). |
| `charging` | INTEGER | NOT NULL DEFAULT 0 | 0/1. `state.powerSupply.charging`. |
| `suspended` | INTEGER | NOT NULL DEFAULT 0 | 0/1. Excluded from new order assignment. |
| `archived` | INTEGER | NOT NULL DEFAULT 0 | 0/1. Hidden from default listings. Irreversible. |
| `factsheet` | JSON | DEFAULT NULL | **Embedded JSON.** Complete VDA 5050 factsheet message. |
| `last_updated_at` | TEXT | NOT NULL | ISO-8601. `state.timestamp`. |
| `created_at` | TEXT | NOT NULL | ISO-8601. |

**Indexes**: `idx_robots_site` on `site_id`.

**Embedded JSON -- factsheet**: Contains the full VDA 5050 factsheet reported
by the robot after a `factsheetRequest` instant action. Includes
`typeSpecification`, `physicalParameters`, `protocolLimits`,
`protocolFeatures`, `mobileRobotGeometry`, `loadSpecification`,
`mobileRobotConfiguration`. Read as a whole via
`GET /robots/{robotId}/factsheet`.

---

### orders (Migration 002)

Stores VDA 5050 orders with management-side metadata. The `nodes` and `edges`
arrays are embedded as JSON to preserve the full VDA 5050 order structure.

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| `order_id` | TEXT | PRIMARY KEY | VDA 5050 order identifier. |
| `order_update_id` | INTEGER | NOT NULL DEFAULT 0 | VDA 5050 `orderUpdateId`. Auto-incremented on each update. |
| `order_description` | TEXT | DEFAULT NULL | Human-readable description. |
| `status` | TEXT | NOT NULL DEFAULT `'QUEUED'` | `QUEUED` / `RUNNING` / `SUCCEEDED` / `FAILED` / `CANCELED`. |
| `assigned_robot_id` | TEXT | NOT NULL | FK to `robots.robot_id`. |
| `site_id` | TEXT | NOT NULL | Immutable for the order lifetime. |
| `nodes` | JSON | NOT NULL | **Embedded JSON.** VDA 5050 `order.nodes` array. |
| `edges` | JSON | NOT NULL | **Embedded JSON.** VDA 5050 `order.edges` array. |
| `archived` | INTEGER | NOT NULL DEFAULT 0 | 0/1. Irreversible; only terminal-state orders. |
| `created_at` | TEXT | NOT NULL | ISO-8601. |
| `last_updated_at` | TEXT | NOT NULL | ISO-8601. |

**Indexes**: `idx_orders_robot` on `assigned_robot_id`,
`idx_orders_site` on `site_id`,
`idx_orders_status` on `status`.

**Embedded JSON -- nodes / edges**: Contain the full VDA 5050 order content
including `actions`, `nodePosition`, `trajectory`, `corridor`, etc.
Not split into child tables.

---

### instant_actions (Migration 003)

Stores the history of instant actions triggered against robots. Each record
captures the full VDA 5050 action payload and its lifecycle status.

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| `action_id` | TEXT | PRIMARY KEY | Server-generated unique ID. |
| `action_type` | TEXT | NOT NULL | e.g. `cancelOrder`, `startPause`, `factsheetRequest`. |
| `action_descriptor` | TEXT | DEFAULT NULL | Human-readable description. |
| `blocking_type` | TEXT | NOT NULL DEFAULT `'NONE'` | Always `NONE` per VDA 5050 instantActions. |
| `action_parameters` | JSON | DEFAULT NULL | **Embedded JSON.** VDA 5050 `actionParameters` array. |
| `retriable` | INTEGER | NOT NULL DEFAULT 0 | 0/1. |
| `action_status` | TEXT | NOT NULL DEFAULT `'WAITING'` | `WAITING` / `INITIALIZING` / `RUNNING` / `PAUSED` / `RETRIABLE` / `FINISHED` / `FAILED`. |
| `action_result` | TEXT | DEFAULT NULL | Result description on completion. |
| `robot_id` | TEXT | NOT NULL | FK to `robots.robot_id`. |
| `site_id` | TEXT | DEFAULT NULL | Site context at creation time. |
| `archived` | INTEGER | NOT NULL DEFAULT 0 | 0/1. Irreversible; only terminal-state actions. |
| `created_at` | TEXT | NOT NULL | ISO-8601. |
| `last_updated_at` | TEXT | NOT NULL | ISO-8601. |

**Indexes**: `idx_instant_actions_robot` on `robot_id`,
`idx_instant_actions_site` on `site_id`,
`idx_instant_actions_status` on `action_status`.

**Embedded JSON -- action_parameters**: Contains the VDA 5050
`actionParameter` array (`key` + polymorphic `value`). Read as a whole.

---

### maps (Migration 004)

Stores site-scoped map metadata. Graph and zone set details are not stored
in this table; instead, `graph_source` and `zone_set_source` reference
Spatial API Collection IDs where the actual data resides.

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| `map_id` | TEXT | NOT NULL | Map identifier. |
| `map_version` | TEXT | NOT NULL | Map version string. |
| `map_descriptor` | TEXT | DEFAULT NULL | Human-readable name. |
| `map_status` | TEXT | NOT NULL DEFAULT `'ENABLED'` | `ENABLED` / `DISABLED`. |
| `site_id` | TEXT | NOT NULL | Site identifier. |
| `graph_source` | TEXT | DEFAULT NULL | Spatial API Collection ID for navigation graph data. |
| `zone_set_source` | TEXT | DEFAULT NULL | Spatial API Collection ID for zone set data. |
| `last_updated_at` | TEXT | NOT NULL | ISO-8601. |

**Primary key**: `(map_id, site_id)`.

**Indexes**: `idx_maps_site` on `site_id`.

**External references**: `graph_source` and `zone_set_source` point to
Spatial API collections. The daemon fetches graph and zone set data on
demand from the Spatial API; no local copy is stored.

---

## Entities Not Persisted

| Entity | Reason |
|--------|--------|
| `sites` | Used as a plain string dimension (`site_id`) across tables. |
| `graph_nodes` / `graph_edges` | Map sub-resources; referenced via `maps.graph_source`. |
| `zone_sets` / `zones` | Map sub-resources; referenced via `maps.zone_set_source`. |
| Spatial tables (collections, features, routes) | Out of scope for current phase. |
| `FleetOverview` | Computed at query time from `robots` + `orders`. |
| VDA 5050 real-time state | Delivered via MQTT; not persisted. |
| MRIS reports | Update `robots` fields in real time; not persisted separately. |
| Map binary resources (PNG tiles, point clouds) | File-system storage; not in database. |
