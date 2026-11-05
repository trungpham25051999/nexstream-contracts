#include <fstream>
#include <iostream>
#include "nexstream/transport/v1/transport.pb.h"
int main(int argc, char** argv) {
    if (argc != 2) { std::cerr << "usage: generate_golden OUTPUT\n"; return 2; }
    nexstream::transport::v1::Frame frame;
    frame.set_protocol_version(1);
    auto* value = frame.mutable_telemetry()->mutable_snapshot();
    value->set_host_id("golden-host-01");
    value->set_message_id("golden-message-01");
    value->set_sample_timestamp_ms(1'800'000'000'000);
    value->set_cpu_usage_percent(12.5);
    value->set_memory_used_bytes(4'096);
    value->set_memory_total_bytes(8'192);
    std::ofstream output(argv[1], std::ios::binary);
    return frame.SerializeToOstream(&output) && output.good() ? 0 : 1;
}
