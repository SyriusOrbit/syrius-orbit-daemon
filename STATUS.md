# Project Status

## Granularity Levels

Progress items are tracked at four levels of granularity, from coarsest to finest:

| Level | Name | Description | Example |
|-------|------|-------------|---------|
| L1 | **Milestone** | A independently verifiable phase outcome | Complete Demo |
| L2 | **Feature** | A deliverable unit scoped by API or component | Complete Fleet API |
| L3 | **Task** | A concrete implementable work item | Create VDA5050Proxy class |
| L4 | **Chore** | A small code-level fix or maintenance task | Fix MQTT connection timeout bug |

Items are ordered newest to oldest. A Milestone is placed at the top of its feature group, with its features ordered newest-first below.

Completion dates are written as `[YYYY-MM-DD]` at the end of the line. Items without a recorded date use `[--]` as a placeholder. Completion dates appear only in the **Recently Completed** section; the **Current Status** section lists items without dates.

Hierarchy is expressed through indentation: 2 spaces per level. Milestones (L1) have no indent, Features (L2) are indented 2 spaces, Tasks (L3) 4 spaces, and Chores (L4) 6 spaces. A Feature is always listed under its parent Milestone, and a Task under its parent Feature.

## ID Format

Each item has a stable hierarchical ID assigned at creation and never renumbered. The ID encodes the full ancestry; subscripts increment in creation order within the parent (oldest = 1). Display order in STATUS.md remains newest-first regardless of ID.

| Level | Format | Example |
|-------|--------|---------|
| L1 Milestone | `M<n>` | `M3` |
| L2 Feature | `M<n>F<f>` | `M3F4` |
| L3 Task | `M<n>F<f>T<t>` | `M3F4T1` |
| L4 Chore | `M<n>F<f>T<t>C<c>` | `M3F4T1C1` |

## Current Status

- [M4] Simulator integration & Fleet data display
  - [M4F1] Write model integration
    - [M4F1T1] Inject `Vda5050EventsRepository` into `VDA5050Proxy` callback chain
    - [M4F1T2] Implement `Vda5050Event` construction from MQTT topic metadata and serialized payload
    - [M4F1T3] Verify write model: start daemon with simulator, confirm `vda5050_events` table has records
  - [M4F2] Projection layer validation
    - [M4F2T1] Verify all 6 projectors correctly update read model tables from simulator messages
    - [M4F2T2] Verify projection failure isolation: malformed payload is skipped without affecting other projections
  - [M4F3] Fleet HTTP API
    - [M4F3T1] Create `FleetApiService` component and register routes on daemon HTTP server
    - [M4F3T2] Implement `GET /robots` and `GET /robots/{robotId}`
    - [M4F3T3] Implement `GET /orders` and `GET /orders/{orderId}`
    - [M4F3T4] Implement `GET /instant-actions`
    - [M4F3T5] Implement `GET /events` with topic/serial_number/direction query filters for message history log
    - [M4F3T6] Integrate `FleetApiService` into `Daemon::run()` lifecycle
  - [M4F4] Fleet web UI
    - [M4F4T1] Replace mock data in `app.js` with real Fleet HTTP API calls (robots, orders, actions)
    - [M4F4T2] Adapt message history log to consume `GET /events` API instead of in-memory mock data

## Recently Completed

- [M3] Data model and storage [2026-08-19]
  - [M3F5] Projection layer [2026-08-19]
  - [M3F4] Base data access layer [2026-08-18]
  - [M3F3] Schema migration system [2026-08-17]
  - [M3F2] Core table schema design [2026-08-16]
  - [M3F1] SQLite dependency and build integration [2026-08-14]
- [M2] AI infrastructure established [--]
  - [M2F3] Progress tracking file created (STATUS.md) [--]
  - [M2F2] Architecture documentation created (ARCHITECTURE.md) [--]
  - [M2F1] AI agent workflow and rules defined (AI_SPEC.md) [--]
- [M1] Core implementation framework [--]
  - [M1F9] Demo web page for health check [--]
  - [M1F8] SpatialService with HTTP endpoint [--]
  - [M1F7] VDA5050Proxy for MQTT bridge (local ↔ cloud) [--]
  - [M1F6] VDA5050 message handling (publisher/subscriber) [--]
  - [M1F5] MQTT infrastructure (publisher/subscriber) [--]
  - [M1F4] VDA5050 JSON schema headers generated [--]
  - [M1F3] Add syrius-orbit-contracts as submodule [--]
  - [M1F2] Command-line and config file configuration parsing [--]
  - [M1F1] Basic project skeleton and CMake build system [--]
