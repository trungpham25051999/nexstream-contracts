# nexstream-contracts — processing guide

Sprint 01 defines and generates the versioned Protobuf envelope, telemetry schema, result codes,
semantic validation, and golden compatibility vector. No runtime network processing exists here.

- Contract version: `nexstream.transport.v1`, `nexstream.telemetry.v1`
- Input/output: `.proto` source to build-tree C++ generated sources
- Success/error tests: `TC-PROTO-001..006`
- Evidence: `tests/results/2026-11-06-sprint-01/`
- Known limitation: compatibility is tested against the checked-in v1 vector; no external schema
  registry or `buf` baseline is configured.
- Next action: review/tag v1 contract, then begin Sprint 02 in `nexstream-ingest`.
