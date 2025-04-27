cmake_minimum_required(VERSION 3.16)
project(XAlgoNexus LANGUAGES CXX)

# ————————————————————————————————————————————————————————————————————————
# Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Default to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Warnings as errors
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
elseif(MSVC)
  add_compile_options(/W4 /WX)
endif()

# ————————————————————————————————————————————————————————————————————————
# Where to find headers
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/third_party
)

# ————————————————————————————————————————————————————————————————————————
# Manually specify all source files
set(XALGO_SOURCES
  ${CMAKE_SOURCE_DIR}/src/main.cpp
  ${CMAKE_SOURCE_DIR}/src/core/ingest/MarketIngestor.cpp
  ${CMAKE_SOURCE_DIR}/src/core/preprocess/TickPreprocessor.cpp
  ${CMAKE_SOURCE_DIR}/src/core/spread/SpreadEngine.cpp
  ${CMAKE_SOURCE_DIR}/src/core/signal/SignalGenerator.cpp
  ${CMAKE_SOURCE_DIR}/src/core/signal/MLFilters/EWMAFilter.cpp
  ${CMAKE_SOURCE_DIR}/src/core/signal/MLFilters/EntropyFilter.cpp
  ${CMAKE_SOURCE_DIR}/src/core/execution/ExecutionEngine.cpp
  ${CMAKE_SOURCE_DIR}/src/core/logger/TradeLogger.cpp
  ${CMAKE_SOURCE_DIR}/src/core/messaging/MessageBus.cpp
  ${CMAKE_SOURCE_DIR}/src/core/risk/RiskManagerThread.cpp
  ${CMAKE_SOURCE_DIR}/src/VolatilityEstimator.cpp
  ${CMAKE_SOURCE_DIR}/src/core/signal/MLFilters/EWMAFilter.cpp
  ${CMAKE_SOURCE_DIR}/src/core/signal/MLFilters/EntropyFilter.cpp
  ${CMAKE_SOURCE_DIR}/src/core/signal/MLFilters/KalmanFilter.cpp
  ${CMAKE_SOURCE_DIR}/src/core/execution/ExecutionEngine.cpp


)

# ————————————————————————————————————————————————————————————————————————
# Define the executable
add_executable(XAlgoNexus ${XALGO_SOURCES})

# Place the binary in build/bin/
set_target_properties(XAlgoNexus PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Link Threads (for std::thread etc.)
find_package(Threads REQUIRED)
target_link_libraries(XAlgoNexus PRIVATE Threads::Threads)

# ————————————————————————————————————————————————————————————————————————
# Copy your data files into the build tree
set(DATA_DIR "${CMAKE_SOURCE_DIR}/data")
file(GLOB DATA_FILES "${DATA_DIR}/*.csv")
foreach(_f IN LISTS DATA_FILES)
  configure_file(${_f} ${CMAKE_BINARY_DIR}/data/ COPYONLY)
endforeach()

# ————————————————————————————————————————————————————————————————————————
# Status messages
message(STATUS "Project configured successfully.")
message(STATUS "Data directory: ${DATA_DIR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
