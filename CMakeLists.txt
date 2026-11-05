cmake_minimum_required(VERSION 3.20)
project(nexstream_contracts VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

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
set_target_properties(nexstream_contracts_proto PROPERTIES EXPORT_NAME contracts_proto)
target_link_libraries(nexstream_contracts_proto PUBLIC protobuf::libprotobuf)
target_include_directories(nexstream_contracts_proto PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/proto>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/proto>")
target_compile_options(nexstream_contracts_proto PRIVATE -Wall -Wextra -Wpedantic)

add_library(nexstream_contracts_validation src/validation.cpp)
add_library(nexstream::contracts_validation ALIAS nexstream_contracts_validation)
set_target_properties(nexstream_contracts_validation PROPERTIES EXPORT_NAME contracts_validation)
target_link_libraries(nexstream_contracts_validation PUBLIC nexstream_contracts_proto)
target_include_directories(nexstream_contracts_validation PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_compile_options(nexstream_contracts_validation PRIVATE -Wall -Wextra -Wpedantic)

include(CTest)
if(BUILD_TESTING)
    add_executable(nexstream_contracts_unit_tests tests/unit/contract_tests.cpp)
    target_link_libraries(nexstream_contracts_unit_tests PRIVATE nexstream_contracts_validation)
    target_include_directories(nexstream_contracts_unit_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_options(nexstream_contracts_unit_tests PRIVATE -Wall -Wextra -Wpedantic)
    add_test(NAME contracts_unit COMMAND nexstream_contracts_unit_tests)
    set_tests_properties(contracts_unit PROPERTIES LABELS "unit")

    add_executable(nexstream_contracts_integration_tests tests/integration/golden_vector_tests.cpp)
    target_link_libraries(nexstream_contracts_integration_tests PRIVATE nexstream_contracts_validation)
    target_include_directories(nexstream_contracts_integration_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_options(nexstream_contracts_integration_tests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_definitions(nexstream_contracts_integration_tests PRIVATE
        NEXSTREAM_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    add_test(NAME contracts_golden_vectors COMMAND nexstream_contracts_integration_tests)
    set_tests_properties(contracts_golden_vectors PROPERTIES LABELS "integration")

    add_executable(nexstream_contracts_generate_golden EXCLUDE_FROM_ALL
        tests/tools/generate_golden.cpp)
    target_link_libraries(nexstream_contracts_generate_golden PRIVATE nexstream_contracts_proto)
endif()

install(TARGETS nexstream_contracts_proto nexstream_contracts_validation
    EXPORT nexstream_contractsTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/proto/nexstream"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/proto"
    FILES_MATCHING PATTERN "*.h")
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY proto/ DESTINATION "${CMAKE_INSTALL_DATADIR}/nexstream-contracts/proto")
install(EXPORT nexstream_contractsTargets
    FILE nexstream_contractsTargets.cmake
    NAMESPACE nexstream::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/nexstream_contracts")
configure_package_config_file(
    cmake/nexstream_contractsConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/nexstream_contractsConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/nexstream_contracts")
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/nexstream_contractsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/nexstream_contractsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/nexstream_contractsConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/nexstream_contracts")
