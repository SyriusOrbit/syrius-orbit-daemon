-- Migration 003: Create maps table.
--
-- Stores lightweight map metadata for the fleet management translation
-- views. Graph and zone set content is not persisted here; it is
-- translated in real time from the Spatial API on each read request.
-- graph_source and zone_set_source hold optional references to the Spatial
-- API collections that provide the graph and zone set data.
--
-- Site is a namespace for maps: maps are always accessed via
-- /sites/{siteId}/maps and there is no global listing, so the primary key
-- is the composite (site_id, map_id).

CREATE TABLE maps (
    site_id           TEXT    NOT NULL,                  -- site namespace
    map_id            TEXT    NOT NULL,                  -- VDA 5050 mapId
    map_version       TEXT    NOT NULL,                  -- map version
    map_descriptor    TEXT    DEFAULT NULL,              -- user-defined descriptor
    map_status        TEXT    NOT NULL DEFAULT 'ENABLED',-- ENABLED/DISABLED
    graph_source      TEXT    DEFAULT NULL,              -- Spatial API collection reference
    zone_set_source   TEXT    DEFAULT NULL,              -- Spatial API collection reference
    last_updated_at   TEXT    DEFAULT NULL,              -- ISO-8601
    PRIMARY KEY (site_id, map_id)
);
