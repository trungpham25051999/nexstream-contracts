# nexstream-contracts

Versioned Protobuf contracts for NexStream transport and telemetry. Sprint 01 is implemented;
this repository contains no Gateway, Kafka, collector, or deployment runtime.

## Requirements

- Linux, CMake 3.20+, a C++20 compiler
- Protobuf compiler and C++ development library 3.21+

Verified locally with CMake 3.25.1, GCC 12.2.0, and Protobuf 3.21.12.

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
cmake --install build --prefix /absolute/install/prefix
```

Consumers use `find_package(nexstream_contracts 0.1 CONFIG REQUIRED)` and link
`nexstream::transport_proto`, `nexstream::telemetry_proto`, or
`nexstream::contracts_validation`. The installed package is versioned 0.1.0.

Generated `.pb.h`/`.pb.cc` files are build artifacts under `build/proto`; they are not committed.
See [BUILDING.md](BUILDING.md), [protocol contract](docs/PROTOCOL.md), and
[actual test evidence](tests/results/README.md).

## Contract contents

- `nexstream.telemetry.v1.SystemMetricsSnapshot`: stable host/message identity, timestamp,
  minimal CPU and memory fields.
- `nexstream.transport.v1.Frame`: distinct auth, batch, telemetry, ACK, and protocol-error
  oneof variants.
- Semantic validation for protocol version, required values, identifier/secret/detail sizes,
  sample count, result enums, and basic metric ranges.

Wire framing remains four-byte unsigned big-endian payload length plus serialized `Frame`.
Framing and Kafka integration remain outside this repository.
