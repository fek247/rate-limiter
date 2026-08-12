cmake_minimum_required(VERSION 3.15)
project(RateLimiter CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(yaml-cpp CONFIG REQUIRED)
find_package(redis++ CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(Crow REQUIRED)

set(PROTO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proto/rate_limiter.proto")
set(GENERATED_PROTO_SRCS 
    "${CMAKE_CURRENT_BINARY_DIR}/rate_limiter.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/rate_limiter.pb.h"
    "${CMAKE_CURRENT_BINARY_DIR}/rate_limiter.grpc.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/rate_limiter.grpc.pb.h"
)

add_custom_command(
    OUTPUT ${GENERATED_PROTO_SRCS}
    COMMAND $<TARGET_FILE:protobuf::protoc>
    ARGS --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
         --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
         --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
         -I "${CMAKE_CURRENT_SOURCE_DIR}/proto"
         "${PROTO_FILE}"
    DEPENDS "${PROTO_FILE}"
)

set(SOURCES 
    src/main.cpp 
    src/parser/parser.cpp
    src/network/tcp_server.cpp
    src/network/thread_pool.cpp
    src/http/response.cpp
    src/api/crow_service.cpp
    ${GENERATED_PROTO_SRCS}
)

add_executable(rate_limiter ${SOURCES})

target_include_directories(rate_limiter PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(rate_limiter PRIVATE 
    yaml-cpp::yaml-cpp 
    redis++::redis++_static
    gRPC::grpc++
    protobuf::libprotobuf
    Crow::Crow
)