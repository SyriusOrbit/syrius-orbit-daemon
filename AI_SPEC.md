# AI Specification for syrius-orbit-daemon

This document defines AI-facing context, authoring rules, and execution workflow for the `syrius-orbit-daemon` repository.

## Scope and Source of Truth

[`syrius-orbit-contracts`](https://github.com/SyriusOrbit/syrius-orbit-contracts) is the single source of truth for all interface contracts in the SyriusOrbit platform.

This daemon repository is an implementation of those contracts. It provides the edge runtime that realizes the behavior defined by the contracts.

All changes to this repository must remain consistent with the corresponding contracts.

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

## Commit Message

Follow the [recommendations from `git-commit`](https://git-scm.com/docs/git-commit#_discussion):

```
<subject>          # 50 chars max, imperative, capitalized, no period

<body>             # 72 chars wrap, explain why not what, present tense
```

The blank line between subject and body is critical for tools like `rebase`.

- **Subject**: imperative mood ("Fix bug" not "Fixed bug"), capitalized, no trailing period, 50 chars max.
- **Body**: wrap at 72 chars, explain the problem and why this change is better (not what the diff already shows), present tense.
- **Trailer** (optional): `Signed-off-by`, `Co-authored-by`, etc. at the end of the body.

## Agent Workflow

When an AI agent updates this repository, use the workflow below:

1. **Read related files** before editing. Understand existing code, neighboring modules, and the relevant contracts.
2. **Make the smallest change set** that satisfies the request.
3. **Preserve naming style, file organization, and existing conventions** as defined in this document.
4. **Verify structural consistency** across related source files and contracts.
5. **Summarize** what changed, why it changed, and what remains out of scope.

## Validation and Definition of Done

A change is complete only when all checks below pass:

- **No impact on existing functionality**: The change does not break existing behavior.
- **No unrelated files changed**: Only files directly relevant to the task are modified.
- **Consistent with contracts**: The implementation aligns with the interface definitions in `syrius-orbit-contracts`.

## Change Boundaries and Safety Rules

- Do not modify contents under `contracts/`. That directory is a separate submodule maintained in its own repository.
- Do not modify code you do not fully understand.
- Do not introduce breaking changes unless explicitly requested.
- Do not add unnecessary abstractions, helper utilities, or design patterns beyond what is needed.
- **Do not modify code without explicit user authorization or a clear indication of intent.** All changes must be discussed with the user and the plan agreed upon before implementation.

## Language Policy

The entire SyriusOrbit project is English-only.

- Do not use any non-English text in code, comments, documentation, or AI-generated content.
- Keep identifiers, summaries, and descriptions fully in English.
- If non-English content is found, replace it with equivalent English text.

This policy is mandatory for both human-authored and AI-generated artifacts.

## Quick Checklist for AI Agents

Before finalizing any change, confirm all items below:

- [ ] Scope is aligned with the daemon's role as an implementation of `syrius-orbit-contracts`
- [ ] Code style deviations (pragma once, PascalCase filenames, .hpp extension, exception policy) are followed
- [ ] Change does not affect existing functionality
- [ ] Only task-relevant files are modified
- [ ] Implementation is consistent with the relevant contracts
- [ ] Output text is fully English
- [ ] Change summary is explicit and limited to actual edits