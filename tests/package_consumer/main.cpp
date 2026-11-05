#include "nexstream/contracts/v1/validation.hpp"
int main() {
    nexstream::transport::v1::Frame frame;
    frame.set_protocol_version(nexstream::contracts::v1::kProtocolVersion);
    return nexstream::contracts::v1::validate_frame(frame) ==
                   nexstream::contracts::v1::ValidationError::kMissingBody
               ? 0 : 1;
}
