# Project Status

## Granularity Levels

Progress items are tracked at four levels of granularity, from coarsest to finest:

| Level | Name | Description | Example |
|-------|------|-------------|---------|
| L1 | **Milestone** | A independently verifiable phase outcome | Complete Demo |
| L2 | **Feature** | A deliverable unit scoped by API or component | Complete Fleet API |
| L3 | **Task** | A concrete implementable work item | Create VDA5050Proxy class |
| L4 | **Chore** | A small code-level fix or maintenance task | Fix MQTT connection timeout bug |

Completion dates are written as `[YYYY-MM-DD]`. Items without a recorded date use `[--]` as a placeholder.

Items are ordered newest to oldest. A Milestone is placed at the top of its feature group, with its features ordered newest-first below.

## ID Format

Each item has a stable hierarchical ID assigned at creation and never renumbered. The ID encodes the full ancestry; subscripts increment in creation order within the parent (oldest = 1). Display order in STATUS.md remains newest-first regardless of ID.

| Level | Format | Example |
|-------|--------|---------|
| L1 Milestone | `M<n>` | `M3` |
| L2 Feature | `M<n>F<f>` | `M3F4` |
| L3 Task | `M<n>F<f>T<t>` | `M3F4T1` |
| L4 Chore | `M<n>F<f>T<t>C<c>` | `M3F4T1C1` |

## Current Status

- [M3] Data model and storage
  - [M3F4] Base data access layer

## Recently Completed

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
