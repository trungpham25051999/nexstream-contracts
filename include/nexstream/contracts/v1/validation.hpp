#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include "nexstream/transport/v1/transport.pb.h"

namespace nexstream::contracts::v1 {
inline constexpr std::size_t kMaxFrameBytes = 65'536;
inline constexpr std::size_t kMaxAuthFrameBytes = 4'096;
inline constexpr std::size_t kMaxIdentifierBytes = 128;
inline constexpr std::size_t kMaxSecretBytes = 512;
inline constexpr std::size_t kMaxDetailBytes = 512;
inline constexpr std::size_t kMaxBatchSamples = 50;
inline constexpr std::size_t kMaxBatchBytes = 1'048'576;
inline constexpr std::uint32_t kProtocolVersion = 1;

enum class ValidationError {
    kNone, kMissingBody, kUnsupportedVersion, kFrameTooLarge, kAuthFrameTooLarge,
    kIdentifierTooLong, kSecretTooLong, kDetailTooLong, kBatchTooLarge,
    kMissingRequiredValue, kInvalidMetric, kInvalidResultCode,
};
[[nodiscard]] ValidationError validate_frame(const nexstream::transport::v1::Frame& frame);
[[nodiscard]] std::string to_string(ValidationError error);
}  // namespace nexstream::contracts::v1
