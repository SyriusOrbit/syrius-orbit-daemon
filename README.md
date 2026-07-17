# syrius-orbit-daemon

`syrius-orbit-daemon` is the local edge daemon implementation for SyriusOrbit.

SyriusOrbit is a spatial information and scheduling platform for indoor Autonomous Mobile Robots (AMRs) built on an edge-cloud collaborative architecture. In that architecture, this repository represents the C++ daemon running on the edge side.

This repository is one implementation of the contracts defined in [`syrius-orbit-contracts`](https://github.com/SyriusOrbit/syrius-orbit-contracts). It is intended to provide the local runtime that stays aligned with the platform's Spatial API and Fleet API contracts.

## Platform Context

SyriusOrbit is designed as a hybrid edge-cloud system:

- Edge side:
  - local C++ daemon
  - lightweight C++ SDK for robot business applications
- Cloud side:
  - Java Spring Cloud microservices for enterprise capabilities

Within that architecture, the daemon is the edge-side process responsible for low-latency local interaction and protocol-facing runtime behavior.

## Relationship to `syrius-orbit-contracts`

The contract repository is the single source of truth for platform interfaces.

This daemon is expected to implement the edge-side behavior behind those contracts, especially for:

- robot communication and fleet messaging
- local service integration on the edge side
- consistent interface behavior between edge and cloud deployments

At the current stage, this repository is still an early skeleton and does not yet expose the full contract-facing HTTP or MQTT functionality described by the contracts repository.

## Third-party Dependencies

- `mosquitto`
- `plog`
