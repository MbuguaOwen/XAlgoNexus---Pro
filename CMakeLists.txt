cmake_minimum_required(VERSION 3.10)
project(XAlgoNexus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use fmt from vcpkg
find_package(fmt CONFIG REQUIRED)

# Add include directory for core headers
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add all source files here
add_executable(XAlgoMain
    src/main.cpp
)

target_link_libraries(XAlgoMain PRIVATE fmt::fmt)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)
