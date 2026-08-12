# Architecture of syrius-orbit-daemon

## Architecture Snapshot

SyriusOrbit is a spatial information and scheduling platform for indoor AMRs with edge-cloud collaboration.

The daemon is designed to implement all APIs defined in the contracts. Current completion status:

- **Fleet Control API**: Partial implementation, low overall completion.
- **Spatial API**: Basic skeleton with `GET /health`.
- **Fleet Management API**: Not yet implemented.

### Deployment Modes

The daemon can be deployed in two modes:

| Aspect | Embedded Robot | Edge Node |
|--------|---------------|-----------|
| Default connection limit | 1 | 256 |
| TLS / certificates | Not required | Required |
| Purpose | Low-latency local robot interaction | Multi-robot fleet coordination |

### Core Components

- **Daemon** (`Daemon.hpp`, `Daemon.cpp`): Main lifecycle management, HTTP server, startup/shutdown orchestration.
- **SpatialService** (`SpatialService.hpp`, `SpatialService.cpp`): HTTP route module exposing spatial API endpoints.
- **FleetGateway** (`FleetGateway.hpp`, `FleetGateway.cpp`): Manages the VDA5050 proxy lifecycle.
- **VDA5050Proxy** (`VDA5050Proxy.hpp`, `VDA5050Proxy.cpp`): MQTT bridge between local and cloud endpoints, forwarding VDA5050 messages bidirectionally.

### Component Dependency Relationships

```
Daemon
  ├── FleetGateway
  │     └── VDA5050Proxy
  │           ├── LocalEndpoint (MQTT client, connects to robot broker)
  │           └── CloudEndpoint (MQTT client, connects to cloud broker)
  └── SpatialService (registered as HTTP routes on Daemon's httplib::server)
```

- `Daemon` owns the HTTP server (`httplib::Server`) and the `FleetGateway` instance.
- `FleetGateway` owns the `VDA5050Proxy` instance.
- `VDA5050Proxy` manages two MQTT endpoints (`VDA5050EndPoint`), one for local robot communication and one for cloud communication.
- `SpatialService` receives a reference to `Daemon`'s HTTP server and registers routes on it.

### Data Flow

**MQTT message flow (bidirectional bridge):**

```
Local robot MQTT broker
    │
    │  state, visualization, connection, factsheet (local → cloud)
    ▼
LocalEndpoint (VDA5050Proxy)
    │
    ▼
CloudEndpoint (VDA5050Proxy)
    │
    │  order, instantActions, responses, zoneSet (cloud → local)
    ▼
Cloud MQTT broker
```

- Local endpoint subscribes to robot topics and forwards received messages to the cloud endpoint for publishing.
- Cloud endpoint subscribes to cloud topics and forwards received messages to the local endpoint for publishing.

**HTTP request flow:**

```
Client HTTP request
    │
    ▼
Daemon::run() → httplib::Server
    ├── /health  →  SpatialService (registered route)
    └── /console/*  →  static file serving from web/ directory
```

### Startup and Shutdown Order

Startup:
1. `Daemon::run()` creates and starts the HTTP server.
2. `FleetGateway::init()` and `FleetGateway::start()` initialize and start the VDA5050 proxy.
3. `SpatialService::init()` and `SpatialService::bindRoutes()` register routes on the HTTP server.

Shutdown (reverse order):
1. `SpatialService` is stopped first.
2. `FleetGateway` / `VDA5050Proxy` is stopped second.
3. HTTP server is stopped last.

Error handling:
- Fleet startup failure causes daemon startup to fail.
- Spatial startup failure is logged but does not block daemon startup.

## Directory Structure

```
syrius-orbit-daemon/
├── contracts/            # Git submodule: syrius-orbit-contracts (interface specs)
├── include/
│   └── syrius_orbit/     # Public headers (C++20)
│       ├── mqtt/         #   MQTT client abstractions (VDA5050EndPoint, TypedPublisher, etc.)
│       └── vda5050_schemas/  # VDA5050 data model headers
├── src/                  # Implementation files
├── web/                  # Static frontend assets (served under /console)
├── CMakeLists.txt        # Build configuration
├── conanfile.py          # Conan dependency management
├── AI_SPEC.md            # AI agent rules and workflow
├── AGENTS.md             # AI agent quick reference
├── ARCHITECTURE.md       # This file
├── CLAUDE.md             # Tool-specific agent instructions
└── README.md             # Project overview and usage
```

## Third-party Dependencies

| Dependency | Purpose |
|------------|---------|
| **mosquitto** (libmosquittopp) | MQTT client library for robot and cloud communication |
| **plog** | Lightweight C++ logging library |
| **cpp-httplib** | HTTP server for exposing the Spatial API and static file serving |
| **argparse** | CLI argument parsing for daemon configuration |
| **nlohmann_json** | JSON parsing for configuration files and VDA5050 message serialization |

## Build and Run

> **Note**: This section is a placeholder. Build commands are not yet specified.

## Test and Lint

> **Note**: This section is a placeholder. No test or lint commands are currently available.

## Code Style

The codebase follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with the following deviations:

- **Header guard**: Use `#pragma once` instead of include guards (macro guards).
- **File naming**: File names use PascalCase matching the class name (e.g., `FleetGateway.hpp`), not snake_case.
- **Header extension**: Use `.hpp` for C++ header files.
- **Exceptions**: Limited, cautious use of exceptions is allowed (Google Style generally disallows them).

### Additional Conventions

- **Directory layout**: Header files live under `include/syrius_orbit/`, source files under `src/`.
- **Logging**: Use the `plog` library with `PLOGI` / `PLOGE` / `PLOGD` macros. No specific log message format is enforced.
- **C++ standard**: C++20.