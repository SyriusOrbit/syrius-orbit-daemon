# CQRS Architecture for syrius-orbit-daemon

## 1. Overview

The daemon implements CQRS (Command Query Responsibility Segregation) with an
event stream as the source of truth and a normalized read model serving HTTP
queries.

- **Write model**: a single append-only `vda5050_events` table, with a
  `topic` column distinguishing VDA5050 topics.
- **Read model**: 4 existing tables (robots, orders, instant\_actions, maps).
- **Projection**: periodic (every 1 second), fetches the latest event per
  entity via `MAX(id) GROUP BY item_id WHERE topic = ?`, translates, and
  upserts into the read model.
- **Repository layer**: every table has its own Repository class.

## 2. Core Design Decisions

### 2.1 Data Retention: Time-based Cleanup

- **Strategy**: Retain event stream data for a fixed period (e.g., 7 days).
  A background task will periodically delete records older than the retention
  window.
- **Rationale**: Simplifies implementation and avoids complex
  cross-projection coordination. Event data is considered transient; the
  read model is the long-term source of truth.

### 2.2 Data Flow: Bidirectional Recording

- **Strategy**: Record all messages passing through `VDA5050Proxy`,
  regardless of direction (LocalEndpoint to CloudEndpoint, or vice versa).
- **Rationale**: Ensures a complete, replayable audit trail of all
  inter-component communication.

### 2.3 Projection Failure: Log and Skip

- **Strategy**: When a projection fails to process an event, the error is
  logged and the event is skipped. The read model retains its last
  successfully projected state. The event will be retried on the next
  projection cycle (since it remains the `MAX(id)` for its `item_id`).
  For high-frequency topics (e.g., state), a newer event will naturally
  supersede the failing one within seconds. For low-frequency topics,
  operator intervention may be required.
- **Rationale**: Write table remains pure append-only. No UPDATE or flag
  columns are needed.

### 2.4 Projection: Periodic, Stateless, No Cursor

- **Strategy**: Projection runs every 1 second. For each topic, it
  queries the latest event per entity via `MAX(id) GROUP BY item_id WHERE
  topic = ?`, translates the result, and upserts into the read model. No
  progress tracking, cursor, or persistent state is needed.
- **Rationale**: The read model only needs the latest state per entity.
  Intermediate events are not required. This design eliminates all progress
  tracking infrastructure, memory variables, and schema pollution.

## 3. Data Flow

1. `VDA5050Proxy` receives MQTT messages from both LocalEndpoint (robot) and
   CloudEndpoint.
2. Messages are written to the `vda5050_events` table via the
   `Vda5050EventsRepository`, with `topic` and `item_id` extracted from the
   message.
3. Every 1 second, the **Projection Layer** queries the latest event per
   entity for each topic, translates the result, and upserts into the read
   model tables.
4. HTTP API queries the read model to serve client requests.

## 4. Write Model (Event Stream)

### 4.1 Table: `vda5050_events`

A single append-only table for all VDA5050 messages.

- **Append-only**: No UPDATE or DELETE on event rows (except for time-based
  cleanup).
- **Auto-increment ID**: `id` INTEGER PRIMARY KEY AUTOINCREMENT.

Columns:

| Column            | Type    | Description                                                |
| ----------------- | ------- | ---------------------------------------------------------- |
| id                | INTEGER | PRIMARY KEY AUTOINCREMENT                                  |
| topic             | TEXT    | VDA5050 topic name (connection, state, order, etc.)       |
| item\_id          | TEXT    | Entity key used for `GROUP BY` in projection (see below)  |
| received\_at      | TEXT    | ISO-8601 daemon receive time                               |
| manufacturer      | TEXT    | Extracted from MQTT topic path                             |
| serial\_number    | TEXT    | Extracted from MQTT topic path                             |
| robot\_id         | TEXT    | Composed as `{manufacturer}.{serialNumber}`                |
| direction         | TEXT    | `uplink` (robot → daemon) or `downlink` (daemon → robot)  |
| header\_id        | INTEGER | VDA5050 message header ID                                  |
| header\_version   | TEXT    | VDA5050 protocol version                                   |
| header\_timestamp | TEXT    | Original VDA5050 message timestamp                         |
| payload           | JSON    | Full VDA5050 message                                       |

`item_id` semantics by topic:

| Topic            | `item_id` value          | Extracted from                |
| ---------------- | ------------------------ | ----------------------------- |
| connection       | `robot_id`               | topic path                    |
| state            | `robot_id`               | topic path                    |
| factsheet        | `robot_id`               | topic path                    |
| visualization    | `robot_id`               | topic path                    |
| order            | `orderId`                | `payload`                     |
| instant\_actions | `actionId`               | `payload` (each action row)   |
| responses        | `requestId`              | `payload` (each response row) |
| zone\_set        | `zoneSetId`              | `payload`                     |

- **Snapshot topics** (connection, state, factsheet, visualization): `item_id`
  = `robot_id`. One row per robot, the latest overwrites.
- **Collection topics** (order, instant\_actions, responses, zone\_set):
  `item_id` = business entity ID extracted from `payload`. Multiple rows per
  robot, each entity independently tracked by its latest version.

**Important**: `manufacturer` and `serial_number` are not in the message
payload; they are extracted from the MQTT topic path at write time. The
VDA5050 topic format is:
`{prefix}/{manufacturer}/{serialNumber}/{topic}` (e.g.,
`vda5050/v3/Syrius/SOR-Carrier-001/state`).

## 5. Projection Layer

### 5.1 Mechanism

Projection runs periodically (every 1 second). For each topic, it executes:

```sql
SELECT * FROM vda5050_events
WHERE topic = ?
AND id IN (SELECT MAX(id) FROM vda5050_events WHERE topic = ? GROUP BY item_id)
```

Topic-to-read-table mapping:

| Topic            | Maps to Read Table |
| ---------------- | ------------------ |
| connection       | robots             |
| state            | robots             |
| factsheet        | robots             |
| order            | orders             |
| instant\_actions | instant\_actions   |
| responses        | instant\_actions   |
| visualization    | (deferred)         |
| zone\_set        | (deferred)         |

### 5.2 Properties

- **Stateless**: No cursor, no memory variables, no progress tracking table.
  Projection can restart at any time with zero recovery cost.
- **Idempotent**: All writes to the read model are upserts. Re-running the
  same projection cycle produces the same result.
- **Restart-safe**: On daemon restart, projection immediately picks up the
  latest events. No replay or checkpoint logic needed.
- **Failure-tolerant**: On projection failure, the read model retains its
  last successfully projected state. The failing event is retried on the
  next cycle. For high-frequency topics, a newer event naturally supersedes
  the failing one.

## 6. Repository Layer

Each database table has a corresponding Repository class.

- **Write Repository (1)**: `Vda5050EventsRepository`.
- **Read Repositories (4)**: `RobotsRepository`, `OrdersRepository`,
  `InstantActionsRepository`, `MapsRepository`.

## 7. Implementation Scope

- **M3F4**: Database infrastructure and 4 Read Repositories.
- **Subsequent milestones**: `Vda5050EventsRepository`, `vda5050_events`
  table migration, and Projection layer.

***

**Note**: This document will be updated as the implementation progresses.
