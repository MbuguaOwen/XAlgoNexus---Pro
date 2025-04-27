cmake_minimum_required(VERSION 3.14)            # catch_discover_tests needs ≥3.14
project(XAlgoNexusTests LANGUAGES CXX)

# 1) Pull in Catch2
find_package(Catch2 3 REQUIRED)                 # or 2.13.7 if you’re on v2

# 2) Turn on CTest for test registration
include(CTest)                                  # makes add_test() and friends available
enable_testing()                                # explicitly enable the testing subsystem

# 3) Define your test executable
add_executable(SpreadEngineTests
  tests/SpreadEngineTests.cpp
  # …any other test files…
)

# 4) Link Catch2’s test runner
target_link_libraries(SpreadEngineTests
  PRIVATE Catch2::Catch2WithMain
)

# 5) Tell CMake to auto-discover test cases inside that exe
include(Catch2)                                 # loads the catch_discover_tests() macro
catch_discover_tests(SpreadEngineTests
  # you can pass ARGS, WORKING_DIRECTORY, etc.
)
