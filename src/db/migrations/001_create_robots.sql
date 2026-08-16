-- Migration 001: Create robots table.
--
-- Stores lightweight, high-frequency query fields for fleet management
-- views (RobotSummary). Complete VDA5050 state messages are not persisted;
-- they arrive near real time via MQTT.
--
-- robot_id is composed as "{manufacturer}.{serialNumber}" and is the
-- primary key. site_id is NULL for orphan robots (valid business state).
--
-- suspended and archived are management-side lifecycle flags:
--   suspended: excludes the robot from new order assignment while still
--              receiving MQTT state messages. Set via PATCH.
--   archived:  hides the robot from default listings but keeps it
--              queryable. Archive is irreversible. Set via PATCH.
-- Both default to false and are independently filterable via query params.
--
-- factsheet stores the complete VDA 5050 factsheet message reported by the
-- robot after a factsheetRequest instant action. It is kept as one JSON
-- column to preserve the nested factsheet structure (typeSpecification,
-- physicalParameters, protocolLimits, protocolFeatures,
-- mobileRobotGeometry, loadSpecification, mobileRobotConfiguration).
-- Individual fields are queryable via SQLite JSON1 functions
-- (json_extract, json_each) when needed. The contract only reads the full
-- factsheet via GET /robots/{robotId}/factsheet, so no column split is
-- required.

CREATE TABLE robots (
    robot_id          TEXT    PRIMARY KEY,               -- "{manufacturer}.{serialNumber}"
    manufacturer      TEXT    NOT NULL,                  -- header.manufacturer
    serial_number     TEXT    NOT NULL,                  -- header.serialNumber
    site_id           TEXT    DEFAULT NULL,              -- NULL = orphan, mutable via PATCH
    display_name      TEXT    DEFAULT NULL,              -- management-side field
    connection_state  TEXT    NOT NULL DEFAULT 'OFFLINE',-- VDA5050 connectionState enum
    operating_mode    TEXT    DEFAULT NULL,              -- VDA5050 state.operatingMode
    driving           INTEGER NOT NULL DEFAULT 0,        -- 0/1
    paused            INTEGER NOT NULL DEFAULT 0,        -- 0/1
    battery_level     REAL    DEFAULT NULL,              -- state.powerSupply.stateOfCharge
    charging          INTEGER NOT NULL DEFAULT 0,        -- 0/1
    suspended         INTEGER NOT NULL DEFAULT 0,        -- 0/1, excludes from order assignment
    archived          INTEGER NOT NULL DEFAULT 0,        -- 0/1, hidden from default listings
    factsheet         JSON    DEFAULT NULL,              -- complete VDA 5050 factsheet (JSON)
    last_updated_at   TEXT    NOT NULL,                  -- ISO-8601, state.timestamp
    created_at        TEXT    NOT NULL                   -- ISO-8601
);

-- Index for the common fleet overview listing filter: robots by site.
CREATE INDEX idx_robots_site ON robots(site_id);
