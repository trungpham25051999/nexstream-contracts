# Sprint 01 test cases

| ID | Scenario | Expected | Current status |
|---|---|---|---|
| TC-PROTO-001 | Valid Frame encode/decode | Semantic roundtrip succeeds | PASS |
| TC-PROTO-002 | Unknown optional field | Known data parses unchanged | PASS |
| TC-PROTO-003 | Missing oneof/truncated wire | Semantic/parser rejection | PASS |
| TC-PROTO-004 | Retry identity | `message_id` remains unchanged | PASS |
| TC-PROTO-005 | Auth result vs batch ACK | Distinct oneof cases and enums | PASS |
| TC-PROTO-006 | Oversized ID/secret | Semantic validation rejects | PASS |

Statuses refer to the latest indexed execution, not merely expected results.
