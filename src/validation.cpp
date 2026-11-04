#include "nexstream/contracts/v1/validation.hpp"
#include <cmath>
#include <initializer_list>
#include <string_view>

namespace nexstream::contracts::v1 {
namespace {
bool any_identifier_too_long(std::initializer_list<std::string_view> values) {
    for (const auto value : values) if (value.size() > kMaxIdentifierBytes) return true;
    return false;
}
bool invalid_result_code(nexstream::transport::v1::ResultCode code) {
    return !nexstream::transport::v1::ResultCode_IsValid(static_cast<int>(code)) ||
           code == nexstream::transport::v1::RESULT_CODE_UNSPECIFIED;
}
ValidationError validate_snapshot(const nexstream::telemetry::v1::SystemMetricsSnapshot& value) {
    if (value.host_id().empty() || value.message_id().empty() ||
        value.sample_timestamp_ms() <= 0) return ValidationError::kMissingRequiredValue;
    if (any_identifier_too_long({value.host_id(), value.message_id()}))
        return ValidationError::kIdentifierTooLong;
    if (!std::isfinite(value.cpu_usage_percent()) || value.cpu_usage_percent() < 0.0 ||
        value.cpu_usage_percent() > 100.0 ||
        value.memory_used_bytes() > value.memory_total_bytes())
        return ValidationError::kInvalidMetric;
    return ValidationError::kNone;
}
}  // namespace

ValidationError validate_frame(const nexstream::transport::v1::Frame& frame) {
    using Frame = nexstream::transport::v1::Frame;
    if (frame.protocol_version() != kProtocolVersion) return ValidationError::kUnsupportedVersion;
    if (frame.body_case() == Frame::BODY_NOT_SET) return ValidationError::kMissingBody;
    if (static_cast<std::size_t>(frame.ByteSizeLong()) > kMaxFrameBytes)
        return ValidationError::kFrameTooLarge;
    switch (frame.body_case()) {
        case Frame::kAuthRequest: {
            const auto& value = frame.auth_request();
            if (value.agent_id().empty() || value.host_id().empty() || value.key_id().empty() ||
                value.secret().empty()) return ValidationError::kMissingRequiredValue;
            if (any_identifier_too_long({value.agent_id(), value.host_id(), value.key_id()}))
                return ValidationError::kIdentifierTooLong;
            if (value.secret().size() > kMaxSecretBytes) return ValidationError::kSecretTooLong;
            if (static_cast<std::size_t>(frame.ByteSizeLong()) > kMaxAuthFrameBytes)
                return ValidationError::kAuthFrameTooLarge;
            return ValidationError::kNone;
        }
        case Frame::kAuthResult:
            if (!nexstream::transport::v1::AuthCode_IsValid(frame.auth_result().code()) ||
                frame.auth_result().code() == nexstream::transport::v1::AUTH_CODE_UNSPECIFIED)
                return ValidationError::kInvalidResultCode;
            return frame.auth_result().detail().size() <= kMaxDetailBytes
                       ? ValidationError::kNone : ValidationError::kDetailTooLong;
        case Frame::kBatchStart:
            if (frame.batch_start().batch_id().empty() || frame.batch_start().host_id().empty())
                return ValidationError::kMissingRequiredValue;
            if (any_identifier_too_long({frame.batch_start().batch_id(),
                                         frame.batch_start().host_id()}))
                return ValidationError::kIdentifierTooLong;
            return frame.batch_start().expected_count() <= kMaxBatchSamples
                       ? ValidationError::kNone : ValidationError::kBatchTooLarge;
        case Frame::kTelemetry:
            return frame.telemetry().has_snapshot() ? validate_snapshot(frame.telemetry().snapshot())
                                                    : ValidationError::kMissingRequiredValue;
        case Frame::kBatchEnd:
            if (frame.batch_end().batch_id().empty()) return ValidationError::kMissingRequiredValue;
            if (frame.batch_end().batch_id().size() > kMaxIdentifierBytes)
                return ValidationError::kIdentifierTooLong;
            return frame.batch_end().sent_count() <= kMaxBatchSamples
                       ? ValidationError::kNone : ValidationError::kBatchTooLarge;
        case Frame::kBatchAck:
            if (frame.batch_ack().batch_id().empty()) return ValidationError::kMissingRequiredValue;
            if (frame.batch_ack().batch_id().size() > kMaxIdentifierBytes)
                return ValidationError::kIdentifierTooLong;
            if (invalid_result_code(frame.batch_ack().overall_result()))
                return ValidationError::kInvalidResultCode;
            if (static_cast<std::size_t>(frame.batch_ack().message_results_size()) >
                kMaxBatchSamples) return ValidationError::kBatchTooLarge;
            for (const auto& result : frame.batch_ack().message_results()) {
                if (result.message_id().empty()) return ValidationError::kMissingRequiredValue;
                if (result.message_id().size() > kMaxIdentifierBytes)
                    return ValidationError::kIdentifierTooLong;
                if (invalid_result_code(result.code())) return ValidationError::kInvalidResultCode;
                if (result.detail().size() > kMaxDetailBytes)
                    return ValidationError::kDetailTooLong;
            }
            return ValidationError::kNone;
        case Frame::kProtocolError:
            if (invalid_result_code(frame.protocol_error().code()))
                return ValidationError::kInvalidResultCode;
            return frame.protocol_error().detail().size() <= kMaxDetailBytes
                       ? ValidationError::kNone : ValidationError::kDetailTooLong;
        case Frame::BODY_NOT_SET: break;
    }
    return ValidationError::kMissingBody;
}

std::string to_string(ValidationError error) {
    switch (error) {
        case ValidationError::kNone: return "none";
        case ValidationError::kMissingBody: return "missing body";
        case ValidationError::kUnsupportedVersion: return "unsupported version";
        case ValidationError::kFrameTooLarge: return "frame too large";
        case ValidationError::kAuthFrameTooLarge: return "auth frame too large";
        case ValidationError::kIdentifierTooLong: return "identifier too long";
        case ValidationError::kSecretTooLong: return "secret too long";
        case ValidationError::kDetailTooLong: return "detail too long";
        case ValidationError::kBatchTooLarge: return "batch too large";
        case ValidationError::kMissingRequiredValue: return "missing required value";
        case ValidationError::kInvalidMetric: return "invalid metric";
        case ValidationError::kInvalidResultCode: return "invalid result code";
    }
    return "unknown validation error";
}
}  // namespace nexstream::contracts::v1
