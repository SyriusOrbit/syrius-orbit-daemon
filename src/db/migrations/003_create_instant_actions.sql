-- Migration 003: Create instant_actions table.
--
-- Stores the history of instant actions triggered against robots via
-- POST /robots/{robotId}/instant-actions. Each record captures the
-- full VDA 5050 action payload and its lifecycle status.
--
-- action_id is server-generated (suggested format: UUID). The
-- blocking_type is always NONE per the VDA 5050 instantActions schema.
--
-- action_parameters is an embedded JSON column that stores the VDA 5050
-- actionParameter array (key + polymorphic value). It is not split into
-- a child table; individual parameters are queryable via SQLite JSON1
-- functions when needed.
--
-- robot_id references the target robot. site_id captures the site
-- context at creation time and may be NULL for orphan robots.
--
-- archived hides terminal-state actions (FINISHED, FAILED) from
-- default listings. Archive is irreversible.

CREATE TABLE instant_actions (
    action_id           TEXT    PRIMARY KEY,              -- server-generated unique ID
    action_type         TEXT    NOT NULL,                  -- e.g. cancelOrder, startPause
    action_descriptor   TEXT    DEFAULT NULL,              -- human-readable description
    blocking_type       TEXT    NOT NULL DEFAULT 'NONE',   -- always NONE for instant actions
    action_parameters   JSON    DEFAULT NULL,              -- VDA 5050 actionParameters array (JSON)
    retriable           INTEGER NOT NULL DEFAULT 0,        -- 0/1
    action_status       TEXT    NOT NULL DEFAULT 'WAITING',-- WAITING/INITIALIZING/RUNNING/PAUSED/RETRIABLE/FINISHED/FAILED
    action_result       TEXT    DEFAULT NULL,              -- result description on completion
    robot_id            TEXT    NOT NULL,                  -- FK -> robots.robot_id
    site_id             TEXT    DEFAULT NULL,              -- site context at creation time
    archived            INTEGER NOT NULL DEFAULT 0,        -- 0/1, irreversible
    created_at          TEXT    NOT NULL,                  -- ISO-8601
    last_updated_at     TEXT    NOT NULL                   -- ISO-8601
);

-- Index for listing instant actions by robot.
CREATE INDEX idx_instant_actions_robot ON instant_actions(robot_id);

-- Index for site-scoped instant action listing.
CREATE INDEX idx_instant_actions_site ON instant_actions(site_id);

-- Index for filtering by status (e.g. pending actions).
CREATE INDEX idx_instant_actions_status ON instant_actions(action_status);
