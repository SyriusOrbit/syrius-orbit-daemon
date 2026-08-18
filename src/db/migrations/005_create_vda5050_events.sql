-- Migration 005: Create vda5050_events table.
--
-- Append-only event stream for all VDA5050 messages passing through the
-- VDA5050Proxy. This is the write model in the CQRS architecture.
--
-- Each row captures a single VDA5050 message with its metadata extracted
-- from the MQTT topic path and message header. The payload column stores
-- the full VDA5050 message as JSON for future replay or re-projection.
--
-- The (topic, item_id) index supports the projection query pattern:
--   SELECT * FROM vda5050_events
--   WHERE topic = ?
--   AND id IN (SELECT MAX(id) FROM vda5050_events WHERE topic = ? GROUP BY item_id)
--
-- received_at index supports time-based cleanup.

CREATE TABLE vda5050_events (
    id                INTEGER PRIMARY KEY AUTOINCREMENT,
    topic             TEXT    NOT NULL,        -- VDA5050 topic (connection, state, order, etc.)
    item_id           TEXT    NOT NULL,        -- Entity key for GROUP BY in projection
    received_at       TEXT    NOT NULL,        -- ISO-8601 daemon receive time
    manufacturer      TEXT    NOT NULL,        -- Extracted from MQTT topic path
    serial_number     TEXT    NOT NULL,        -- Extracted from MQTT topic path
    robot_id          TEXT    NOT NULL,        -- "{manufacturer}.{serialNumber}"
    direction         TEXT    NOT NULL,        -- "uplink" or "downlink"
    header_id         INTEGER DEFAULT NULL,    -- VDA5050 message header ID
    header_version    TEXT    DEFAULT NULL,    -- VDA5050 protocol version
    header_timestamp  TEXT    DEFAULT NULL,    -- Original VDA5050 message timestamp
    payload           JSON    NOT NULL         -- Full VDA5050 message
);

-- Index for projection query: latest event per item_id per topic.
CREATE INDEX idx_vda5050_events_topic_item ON vda5050_events(topic, item_id);

-- Index for time-based cleanup.
CREATE INDEX idx_vda5050_events_received_at ON vda5050_events(received_at);