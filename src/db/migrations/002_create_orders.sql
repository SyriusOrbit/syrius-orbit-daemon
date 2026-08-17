-- Migration 002: Create orders table.
--
-- Stores VDA 5050 orders with management-side metadata. Each order is
-- identified by order_id (VDA 5050 order identifier) and tracks its
-- lifecycle through the status column (QUEUED -> RUNNING -> SUCCEEDED /
-- FAILED / CANCELED).
--
-- assigned_robot_id references the robot that executes the order.
-- site_id is immutable for the order lifetime and provides the site
-- scoping dimension for listing and filtering.
--
-- nodes and edges are embedded JSON columns that store the complete
-- VDA 5050 order.nodes and order.edges arrays, including nested
-- actions, nodePosition, trajectory, and corridor structures. They
-- are not split into child tables; individual fields are queryable
-- via SQLite JSON1 functions when needed.
--
-- archived hides terminal-state orders from default listings.
-- Archive is irreversible.

CREATE TABLE orders (
    order_id            TEXT    PRIMARY KEY,              -- VDA 5050 order identifier
    order_update_id     INTEGER NOT NULL DEFAULT 0,       -- VDA 5050 orderUpdateId
    order_description   TEXT    DEFAULT NULL,              -- human-readable description
    status              TEXT    NOT NULL DEFAULT 'QUEUED', -- QUEUED/RUNNING/SUCCEEDED/FAILED/CANCELED
    assigned_robot_id   TEXT    NOT NULL,                  -- FK -> robots.robot_id
    site_id             TEXT    NOT NULL,                  -- immutable for order lifetime
    nodes               JSON    NOT NULL,                  -- VDA 5050 order.nodes array (JSON)
    edges               JSON    NOT NULL,                  -- VDA 5050 order.edges array (JSON)
    archived            INTEGER NOT NULL DEFAULT 0,        -- 0/1, irreversible
    created_at          TEXT    NOT NULL,                  -- ISO-8601
    last_updated_at     TEXT    NOT NULL                   -- ISO-8601
);

-- Index for listing orders by assigned robot.
CREATE INDEX idx_orders_robot ON orders(assigned_robot_id);

-- Index for site-scoped order listing.
CREATE INDEX idx_orders_site ON orders(site_id);

-- Index for filtering orders by status (e.g. active order count).
CREATE INDEX idx_orders_status ON orders(status);
