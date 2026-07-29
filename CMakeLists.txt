cmake_minimum_required(VERSION 3.10)

project(RateLimiter CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(yaml-cpp REQUIRED)

add_executable(rate_limiter 
    main.cpp 
    parser/parser.cpp
)

target_include_directories(rate_limiter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rate_limiter PRIVATE yaml-cpp)