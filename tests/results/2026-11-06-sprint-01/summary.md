# Sprint 01 test execution — 2026-11-06

- Run date: 2026-11-06 (Asia/Ho_Chi_Minh; execution completed during this session)
- Revision: no commit; newly initialized local `main` repository
- OS: Debian GNU/Linux, kernel 6.1.0-40-amd64
- CPU/RAM: Intel Core i7-8550U, 8 logical CPUs, 15 GiB RAM
- Toolchain: GCC 12.2.0, CMake 3.25.1, Protobuf/protoc 3.21.12

| Test ID | Observed result | Status | Executable |
|---|---|---|---|
| TC-PROTO-001 | Valid telemetry Frame serialized, parsed, matched, and validated | PASS | `contracts_unit`, `contracts_golden_vectors` |
| TC-PROTO-002 | Appended unknown field parsed while known message ID remained intact | PASS | `contracts_golden_vectors` |
| TC-PROTO-003 | Missing oneof failed semantic validation; truncated wire failed parsing | PASS | both |
| TC-PROTO-004 | Retry fixture retained identical stable message ID | PASS | `contracts_unit` |
| TC-PROTO-005 | AuthResult and BatchAck used distinct oneof cases/enums | PASS | `contracts_unit` |
| TC-PROTO-006 | 129-byte ID and 513-byte secret were rejected | PASS | `contracts_unit` |
| External schema lint/registry gate | `buf`/registry baseline not installed or configured | BLOCKED | not applicable |

Totals: **PASS 6, FAIL 0, BLOCKED 1, NOT RUN 0**. The sanitizer build also ran both
CTest executables with AddressSanitizer and UndefinedBehaviorSanitizer and reported no failures.
No benchmark was run.
