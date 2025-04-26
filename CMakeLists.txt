cmake_minimum_required(VERSION 3.10)
project(XAlgoNexus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use fmt from vcpkg
find_package(fmt CONFIG REQUIRED)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# Add all source files in ONE executable
add_executable(XAlgoMain
    src/main.cpp
    src/core/messaging/HistoricalReplayIngestor.cpp
    src/core/ingestion/ForexReplayIngestor.cpp
    src/core/spread/SpreadEngine.cpp
)

