-- Migration 004: Create maps table.
--
-- Stores site-scoped map metadata. Each map is identified by the
-- composite key (map_id, site_id).
--
-- This table contains only basic metadata (version, descriptor, status).
-- Navigation graph and zone set details are not stored locally; instead,
-- graph_source and zone_set_source hold Spatial API Collection IDs that
-- reference the collections where the actual data resides. The daemon
-- fetches graph and zone set data on demand from the Spatial API.
--
-- Binary map resources (PNG tiles, point clouds) are stored on the
-- file system; this table does not contain binary data.

CREATE TABLE maps (
    map_id            TEXT    NOT NULL,                   -- map identifier
    map_version       TEXT    NOT NULL,                   -- map version string
    map_descriptor    TEXT    DEFAULT NULL,                -- human-readable name
    map_status        TEXT    NOT NULL DEFAULT 'ENABLED',  -- ENABLED / DISABLED
    site_id           TEXT    NOT NULL,                   -- site identifier
    graph_source      TEXT    DEFAULT NULL,                -- Spatial API Collection ID for graph data
    zone_set_source   TEXT    DEFAULT NULL,                -- Spatial API Collection ID for zone set data
    last_updated_at   TEXT    NOT NULL,                   -- ISO-8601
    PRIMARY KEY (map_id, site_id)
);

-- Index for listing maps by site.
CREATE INDEX idx_maps_site ON maps(site_id);
