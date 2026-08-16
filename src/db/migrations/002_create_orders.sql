-- Migration 002: Create orders table.
--
-- Stores management-side orders following the VDA 5050 order semantics.
-- The VDA 5050 order content (orderId, orderUpdateId, nodes, edges,
-- actions) is preserved; nodes and edges are kept as JSON columns to
-- retain the full nested structure.
--
-- site_id is required at creation (POST /sites/{siteId}/orders) and is
-- immutable for the lifetime of the order. assigned_robot_id references
-- the robot assigned to execute the order.
--
-- status follows the management lifecycle:
-- QUEUED -> RUNNING -> SUCCEEDED/FAILED, or CANCELED. archived marks a
-- terminal order as hidden from default listings; archive is irreversible
-- via the API.

CREATE TABLE orders (
    order_id          TEXT    PRIMARY KEY,               -- VDA 5050 orderId
    order_update_id   INTEGER NOT NULL DEFAULT 0,        -- server auto-increments on update
    order_description TEXT    DEFAULT NULL,              -- human-readable, visualization only
    status            TEXT    NOT NULL DEFAULT 'QUEUED', -- QUEUED/RUNNING/SUCCEEDED/FAILED/CANCELED
    assigned_robot_id TEXT    DEFAULT NULL REFERENCES robots(robot_id),
    site_id           TEXT    NOT NULL,                  -- required at creation, immutable
    nodes             JSON    NOT NULL,                  -- VDA 5050 Node array
    edges             JSON    NOT NULL DEFAULT '[]',     -- VDA 5050 Edge array, may be empty
    archived          INTEGER NOT NULL DEFAULT 0,        -- 0/1, hidden from default listings
    created_at        TEXT    NOT NULL                   -- ISO-8601
);

-- Indexes for the order listing filters and fleet overview aggregates.
CREATE INDEX idx_orders_site ON orders(site_id);
CREATE INDEX idx_orders_status ON orders(status);
CREATE INDEX idx_orders_assigned_robot ON orders(assigned_robot_id);
