# syrius-orbit-daemon

`syrius-orbit-daemon` is the local edge daemon implementation for SyriusOrbit.

SyriusOrbit is a spatial information and scheduling platform for indoor Autonomous Mobile Robots (AMRs) built on an edge-cloud collaborative architecture. In that architecture, this repository represents the C++ daemon running on the edge side.

This repository is one implementation of the contracts defined in [`syrius-orbit-contracts`](https://github.com/SyriusOrbit/syrius-orbit-contracts), and it provides an edge runtime that supports two API suites:

- **SyriusOrbit Spatial API**: OpenAPI-based spatial and geospatial service contracts aligned with the OGC API family.
- **SyriusOrbit Fleet API**: AsyncAPI-based robot communication, scheduling, and fleet messaging contracts aligned with VDA 5050 and related interoperability standards.

## Platform Context

SyriusOrbit is designed as a hybrid edge-cloud system:

- Edge side:
  - local C++ daemon
  - lightweight C++ SDK for robot business applications
- Cloud side:
  - Java Spring Cloud microservices for enterprise capabilities

Within that architecture, the daemon is the edge-side process responsible for low-latency local interaction and protocol-facing runtime behavior.

## Relationship to `syrius-orbit-contracts`

The contract repository is the single source of truth for platform API interfaces and descriptions:

- API contract repository: https://github.com/SyriusOrbit/syrius-orbit-contracts
- API summary and standards: https://github.com/SyriusOrbit/syrius-orbit-contracts#readme

This daemon is expected to implement the edge-side behavior behind those contracts, especially for:

- robot communication and fleet messaging
- local service integration on the edge side
- consistent interface behavior between edge and cloud deployments

At the current stage, this repository is still an early skeleton and does not yet expose the full contract-facing HTTP or MQTT functionality described by the contracts repository.

## Runtime API Exposure

The daemon now starts two edge-facing components managed by the daemon lifecycle:

- `SpatialService` (HTTP route module): registers `GET /health` on the daemon-owned HTTP server.
- Daemon HTTP static hosting: serves `web` directory under `/console`.
- `FleetGateway` (MQTT): starts a `VDA5050Proxy` bridge with two endpoints:
  - local endpoint subscribes `state`, `visualization`, `connection`, `factsheet`, then forwards to cloud.
  - cloud endpoint subscribes `order`, `instantActions`, `responses`, `zoneSet`, then forwards to local.

Startup and shutdown behavior:

- Fleet must start successfully, otherwise daemon startup fails.
- Spatial startup failure is logged but does not fail daemon startup.
- Shutdown order is Spatial first, then Fleet.

Configuration precedence is:

1. CLI arguments
2. Config file
3. Built-in defaults

Default config file path is `syrius_orbit_daemon.json`.

Supported CLI options:

- `--config=<path>`
- `--http-host=<host>`
- `--http-port=<port>`
- `--mqtt-host=<host>`
- `--mqtt-port=<port>`
- `--mqtt-client-id=<id>`
- `--mqtt-username=<username>`
- `--mqtt-password=<password>`
- `--cloud-mqtt-host=<host>`
- `--cloud-mqtt-port=<port>`
- `--cloud-mqtt-client-id=<id>`
- `--cloud-mqtt-username=<username>`
- `--cloud-mqtt-password=<password>`
- `--mqtt-topic-prefix=<prefix>`

`mqtt_topic_prefix` must follow VDA5050 topic root format: `<interfaceName>/<majorVersion>` (for example `vda5050/v3`).

JSON config keys (`snake_case`):

- `http_host`
- `http_port`
- `mqtt_host`
- `mqtt_port`
- `mqtt_client_id`
- `mqtt_username`
- `mqtt_password`
- `cloud_mqtt_host`
- `cloud_mqtt_port`
- `cloud_mqtt_client_id`
- `cloud_mqtt_username`
- `cloud_mqtt_password`
- `mqtt_topic_prefix`

Example `syrius_orbit_daemon.json`:

```json
{
  "http_host": "0.0.0.0",
  "http_port": 8080,
  "mqtt_host": "127.0.0.1",
  "mqtt_port": 1883,
  "mqtt_client_id": "syrius-orbit-daemon",
  "mqtt_username": "",
  "mqtt_password": "",
  "cloud_mqtt_host": "127.0.0.1",
  "cloud_mqtt_port": 1884,
  "cloud_mqtt_client_id": "syrius-orbit-daemon-cloud",
  "cloud_mqtt_username": "",
  "cloud_mqtt_password": "",
  "mqtt_topic_prefix": "vda5050/v3"
}
```

## Third-party Dependencies

- `mosquitto`
- `plog`
- `cpp-httplib`
- `argparse`
- `nlohmann_json`
