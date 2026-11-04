cmake_minimum_required(VERSION 3.20)
project(nexstream_contracts VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Protobuf 3.21 REQUIRED)

set(NEXSTREAM_PROTO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(NEXSTREAM_PROTO_FILES
    "${NEXSTREAM_PROTO_ROOT}/nexstream/telemetry/v1/telemetry.proto"
    "${NEXSTREAM_PROTO_ROOT}/nexstream/transport/v1/transport.proto")
set(PROTOBUF_GENERATE_CPP_APPEND_PATH OFF)
protobuf_generate_cpp(NEXSTREAM_PROTO_SRCS NEXSTREAM_PROTO_HDRS ${NEXSTREAM_PROTO_FILES})

add_library(nexstream_contracts_proto ${NEXSTREAM_PROTO_SRCS} ${NEXSTREAM_PROTO_HDRS})
add_library(nexstream::contracts_proto ALIAS nexstream_contracts_proto)
target_link_libraries(nexstream_contracts_proto PUBLIC protobuf::libprotobuf)
target_include_directories(nexstream_contracts_proto PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/proto>")
target_compile_options(nexstream_contracts_proto PRIVATE -Wall -Wextra -Wpedantic)

add_library(nexstream_contracts_validation src/validation.cpp)
add_library(nexstream::contracts_validation ALIAS nexstream_contracts_validation)
target_link_libraries(nexstream_contracts_validation PUBLIC nexstream_contracts_proto)
target_include_directories(nexstream_contracts_validation PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
target_compile_options(nexstream_contracts_validation PRIVATE -Wall -Wextra -Wpedantic)
