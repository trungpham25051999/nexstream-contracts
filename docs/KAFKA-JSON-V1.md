# Kafka JSON v1 as emitted by Gateway Sprint 04

One JSON object per Kafka record on `telemetry.events.v1`, keyed by `host_id`.
Required fields: integer `schema_version=1`, string `host_id`, string `message_id`,
positive signed 64-bit `sample_timestamp_ms`, finite numeric `cpu_usage_percent` in [0,100],
unsigned 64-bit `memory_used_bytes`, positive unsigned 64-bit `memory_total_bytes` with used <= total.
Identifiers are bounded to 128 bytes by Protobuf semantic validation. Retry preserves
`(host_id,message_id)`. No end-to-end uniqueness or exactly-once guarantee exists.

## Sprint 07 alert representation

`telemetry.alerts.v1`, key `host_id`: `schema_version=1`, `alert_id`, `host_id`,
`message_id`, `sample_timestamp_ms`, `metric` (`cpu` or `ram`), `observed_percent`,
`threshold_percent`, `consecutive_samples`. Alert ID is deterministic
`<host-byte-length>:<host_id><message-byte-length>:<message_id>:<metric>`;
length prefixes prevent colon-containing identities from colliding. Downstream consumers must deduplicate replays.
This new topic representation does not modify Protobuf v1 or telemetry events.

Processor accepts nonempty identifiers up to 128 bytes, matching the Gateway length limits.
Query's URL host validator is currently narrower; see Query README for supported characters.
Malformed records produce metadata-only DLQ events (source topic/partition/offset/reason),
then commit only after successful DLQ delivery. Original payload remains in source Kafka retention.
