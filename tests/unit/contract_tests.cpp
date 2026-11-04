#include <string>
#include "nexstream/contracts/v1/validation.hpp"
#include "tests/test_harness.hpp"

namespace {
using nexstream::contracts::v1::ValidationError;
using nexstream::contracts::v1::validate_frame;
using nexstream::transport::v1::Frame;
Frame valid_telemetry_frame() {
    Frame frame;
    frame.set_protocol_version(1);
    auto* snapshot = frame.mutable_telemetry()->mutable_snapshot();
    snapshot->set_host_id("host-fixture-01");
    snapshot->set_message_id("message-fixture-01");
    snapshot->set_sample_timestamp_ms(1'800'000'000'000);
    snapshot->set_cpu_usage_percent(42.5);
    snapshot->set_memory_used_bytes(4'000);
    snapshot->set_memory_total_bytes(8'000);
    return frame;
}
}  // namespace

int main() {
    TestHarness test;
    const auto source = valid_telemetry_frame();
    std::string encoded;
    test.expect(source.SerializeToString(&encoded), "TC-PROTO-001 serialize valid frame");
    Frame decoded;
    test.expect(decoded.ParseFromString(encoded), "TC-PROTO-001 parse valid frame");
    test.expect(decoded.SerializeAsString() == source.SerializeAsString(),
                "TC-PROTO-001 semantic roundtrip");
    test.expect(validate_frame(decoded) == ValidationError::kNone,
                "TC-PROTO-001 semantic validation");

    Frame missing_body;
    missing_body.set_protocol_version(1);
    test.expect(validate_frame(missing_body) == ValidationError::kMissingBody,
                "TC-PROTO-003 missing oneof rejected");
    const auto retry = valid_telemetry_frame();
    test.expect(retry.telemetry().snapshot().message_id() ==
                    source.telemetry().snapshot().message_id(),
                "TC-PROTO-004 retry preserves message_id");

    Frame auth_result;
    auth_result.set_protocol_version(1);
    auth_result.mutable_auth_result()->set_code(nexstream::transport::v1::AUTHENTICATED);
    Frame batch_ack;
    batch_ack.set_protocol_version(1);
    batch_ack.mutable_batch_ack()->set_batch_id("batch-01");
    batch_ack.mutable_batch_ack()->set_overall_result(nexstream::transport::v1::SUCCESS);
    test.expect(auth_result.body_case() == Frame::kAuthResult &&
                    batch_ack.body_case() == Frame::kBatchAck &&
                    auth_result.body_case() != batch_ack.body_case(),
                "TC-PROTO-005 auth and ack are distinct");

    Frame oversized_id = valid_telemetry_frame();
    oversized_id.mutable_telemetry()->mutable_snapshot()->set_message_id(
        std::string(nexstream::contracts::v1::kMaxIdentifierBytes + 1, 'x'));
    test.expect(validate_frame(oversized_id) == ValidationError::kIdentifierTooLong,
                "TC-PROTO-006 oversized identifier rejected");
    Frame oversized_secret;
    oversized_secret.set_protocol_version(1);
    auto* auth = oversized_secret.mutable_auth_request();
    auth->set_agent_id("agent-01"); auth->set_host_id("host-01"); auth->set_key_id("key-01");
    auth->set_secret(std::string(nexstream::contracts::v1::kMaxSecretBytes + 1, 's'));
    test.expect(validate_frame(oversized_secret) == ValidationError::kSecretTooLong,
                "TC-PROTO-006 oversized secret rejected");
    return test.finish();
}
