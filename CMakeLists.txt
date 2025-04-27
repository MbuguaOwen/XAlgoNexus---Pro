cmake_minimum_required(VERSION 3.10)
project(XAlgoNexus)
add_subdirectory(src/core)
add_subdirectory(tests)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ✅ Correctly find fmt
find_package(fmt CONFIG REQUIRED)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# Add all source files in ONE executable
add_executable(XAlgoMain
    src/main.cpp
)

# Main app links to core
target_link_libraries(XAlgoMain PRIVATE core)


# Unit tests
enable_testing()

