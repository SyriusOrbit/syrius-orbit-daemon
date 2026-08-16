-- Migration 004: Create instant_actions table.
--
-- Stores persisted instant actions triggered via
-- POST /robots/{robotId}/instant-actions. The server generates action_id
-- and tracks the action lifecycle via action_status (WAITING ->
-- INITIALIZING -> RUNNING -> FINISHED/FAILED, with PAUSED and RETRIABLE
-- as intermediate states). blocking_type is always NONE for instant
-- actions, per the VDA 5050 instantActions schema.
--
-- action_parameters keeps the VDA 5050 action parameter list as one JSON
-- column. site_id is the site context at creation time and may be NULL for
-- orphan robots. archived hides terminal actions (FINISHED, FAILED) from
-- default listings; archive is irreversible via the API.

CREATE TABLE instant_actions (
    action_id         TEXT    PRIMARY KEY,               -- server-generated actionId
    action_type       TEXT    NOT NULL,                  -- e.g. cancelOrder, factsheetRequest
    action_descriptor TEXT    DEFAULT NULL,              -- human-readable descriptor
    blocking_type     TEXT    NOT NULL DEFAULT 'NONE',   -- always NONE for instant actions
    action_parameters JSON    DEFAULT NULL,              -- VDA 5050 ActionParameter array
    retriable         INTEGER NOT NULL DEFAULT 0,        -- 0/1
    action_status     TEXT    NOT NULL DEFAULT 'WAITING',-- WAITING/.../FINISHED/FAILED
    action_result     TEXT    DEFAULT NULL,              -- description of the result
    robot_id          TEXT    NOT NULL REFERENCES robots(robot_id),
    site_id           TEXT    DEFAULT NULL,              -- site context at creation, may be NULL
    created_at        TEXT    NOT NULL,                  -- ISO-8601
    last_updated_at   TEXT    NOT NULL,                  -- ISO-8601
    archived          INTEGER NOT NULL DEFAULT 0         -- 0/1, hidden from default listings
);

-- Indexes for the per-robot listing and status filters.
CREATE INDEX idx_instant_actions_robot ON instant_actions(robot_id);
CREATE INDEX idx_instant_actions_status ON instant_actions(action_status);
