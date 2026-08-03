cmake_minimum_required(VERSION 3.15)
project(RateLimiter CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(yaml-cpp CONFIG REQUIRED)
find_package(redis++ CONFIG REQUIRED)
# find_package(Threads REQUIRED)

add_executable(rate_limiter 
    src/main.cpp 
    src/parser/parser.cpp
    src/limiter/redis_limiter.cpp 
    src/network/tcp.cpp
    src/http/response.cpp
)

target_include_directories(rate_limiter PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(rate_limiter PRIVATE 
    yaml-cpp::yaml-cpp 
    redis++::redis++_static
    # Threads::Threads
)