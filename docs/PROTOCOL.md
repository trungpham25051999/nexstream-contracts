# NexStream contracts v1

The TCP payload is a `nexstream.transport.v1.Frame`. A later adapter prefixes each payload with
a four-byte unsigned big-endian length. Package `v1` is the compatibility boundary; field numbers
must never be reused and removed fields must be reserved.

| Value | Limit |
|---|---:|
| Serialized frame | 65,536 bytes |
| Serialized auth frame | 4,096 bytes |
| Agent/host/key/batch/message identifier | 128 bytes each |
| Authentication secret | 512 bytes |
| Human-readable detail | 512 bytes |
| Samples/results per batch | 50 |
| Aggregate decoded batch payload | 1,048,576 bytes |

Protobuf does not enforce semantic limits; consumers must validate after parsing and validate the
frame prefix before allocating. Secrets must never be logged.

Additive unknown fields parse successfully. Unknown enum numbers parse at the Protobuf layer but
fail semantic validation. `AuthResult` uses `AuthCode`; `BatchAck` uses `ResultCode`, preventing
authentication success from being confused with Kafka-confirmed delivery. `BatchAck.SUCCESS` does
not mean ClickHouse persistence. Sprint 01 defines the contract only.

## V1 session semantics clarified for Sprint 03

- `BatchStart.expected_count == 0` means the count is unspecified. It does not mean an empty batch
  and does not disable the maximum 50-sample or 1 MiB batch limits.
- `BatchEnd.sent_count` is always the actual number of Telemetry frames sent in the session. The
  Gateway compares it with the number actually received and with any non-zero `expected_count`.
- A zero/oversized length prefix or malformed Protobuf payload is untrusted framing. The receiver
  closes the connection immediately; it does not send ProtocolError on that stream.
- `BatchAck.UNKNOWN` is not delivery success. A publisher retains the original records and stable
  `(host_id, message_id)` identity for retry. Only `SUCCESS` can represent confirmed delivery, and
  the Sprint 02 Gateway stub intentionally never emits it.
