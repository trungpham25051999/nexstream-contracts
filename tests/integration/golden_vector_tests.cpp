#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include "nexstream/contracts/v1/validation.hpp"
#include "tests/test_harness.hpp"

namespace {
void append_varint(std::string& bytes, std::uint64_t value) {
    while (value >= 0x80U) {
        bytes.push_back(static_cast<char>((value & 0x7fU) | 0x80U));
        value >>= 7U;
    }
    bytes.push_back(static_cast<char>(value));
}
}  // namespace

int main() {
    TestHarness test;
    const std::string path = std::string(NEXSTREAM_SOURCE_DIR) +
                             "/tests/vectors/telemetry_frame_v1.bin";
    std::ifstream input(path, std::ios::binary);
    const std::string golden((std::istreambuf_iterator<char>(input)),
                             std::istreambuf_iterator<char>());
    test.expect(input.good() || input.eof(), "golden fixture can be read");
    nexstream::transport::v1::Frame frame;
    test.expect(frame.ParseFromString(golden), "golden vector parses");
    test.expect(nexstream::contracts::v1::validate_frame(frame) ==
                    nexstream::contracts::v1::ValidationError::kNone,
                "golden vector validates");
    test.expect(frame.telemetry().snapshot().message_id() == "golden-message-01",
                "golden vector carries stable identity");
    test.expect(frame.SerializeAsString() == golden, "golden serialization remains stable");

    std::string with_unknown = golden;
    append_varint(with_unknown, (100U << 3U) | 0U);
    append_varint(with_unknown, 7U);
    nexstream::transport::v1::Frame forward_compatible;
    test.expect(forward_compatible.ParseFromString(with_unknown),
                "TC-PROTO-002 unknown field parses");
    test.expect(forward_compatible.telemetry().snapshot().message_id() ==
                    "golden-message-01",
                "TC-PROTO-002 known data survives unknown field");

    nexstream::transport::v1::Frame unknown_enum;
    unknown_enum.set_protocol_version(1);
    auto* ack = unknown_enum.mutable_batch_ack();
    ack->set_batch_id("batch-01");
    ack->set_overall_result(static_cast<nexstream::transport::v1::ResultCode>(999));
    std::string wire;
    test.expect(unknown_enum.SerializeToString(&wire), "unknown enum serializes");
    nexstream::transport::v1::Frame parsed_unknown_enum;
    test.expect(parsed_unknown_enum.ParseFromString(wire), "unknown enum parses");
    test.expect(nexstream::contracts::v1::validate_frame(parsed_unknown_enum) ==
                    nexstream::contracts::v1::ValidationError::kInvalidResultCode,
                "unknown enum rejected semantically");

    nexstream::transport::v1::Frame malformed;
    test.expect(!malformed.ParseFromString(std::string("\x6a\x05\x0a", 3)),
                "TC-PROTO-003 truncated protobuf rejected");
    return test.finish();
}
