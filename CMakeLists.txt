cmake_minimum_required(VERSION 3.16)
project(XAlgoNexus LANGUAGES CXX)

# ──────────────────────────────────────────────────────────────────────────────
# Basic settings
# ──────────────────────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type." FORCE)
endif()

# ──────────────────────────────────────────────────────────────────────────────
# Treat warnings as errors
# ──────────────────────────────────────────────────────────────────────────────
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# ──────────────────────────────────────────────────────────────────────────────
# Include paths
# ──────────────────────────────────────────────────────────────────────────────
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/third_party
)

# ──────────────────────────────────────────────────────────────────────────────
# Sources
# ──────────────────────────────────────────────────────────────────────────────
set(XALGO_SOURCES
  ${CMAKE_SOURCE_DIR}/src/main.cpp

  ${CMAKE_SOURCE_DIR}/src/core/ingest/HistoricalReplayIngestor.cpp
  ${CMAKE_SOURCE_DIR}/src/core/ingest/ForexReplayIngestor.cpp

  ${CMAKE_SOURCE_DIR}/src/core/preprocess/TickPreprocessor.cpp

  ${CMAKE_SOURCE_DIR}/src/core/spread/SpreadEngine.cpp

  ${CMAKE_SOURCE_DIR}/src/core/signal/SignalGenerator.cpp
  ${CMAKE_SOURCE_DIR}/src/core/signal/MLFilters/EWMAFilter.cpp
  ${CMAKE_SOURCE_DIR}/src/core/signal/MLFilters/EntropyFilter.cpp
  ${CMAKE_SOURCE_DIR}/src/core/signal/MLFilters/KalmanFilter.cpp

  ${CMAKE_SOURCE_DIR}/src/core/execution/ExecutionEngine.cpp

  ${CMAKE_SOURCE_DIR}/src/core/logger/TradeLogger.cpp

  ${CMAKE_SOURCE_DIR}/src/core/messaging/MessageBus.hpp

  ${CMAKE_SOURCE_DIR}/src/core/risk/RiskEngine.cpp
  ${CMAKE_SOURCE_DIR}/src/core/risk/RiskManagerThread.cpp

  ${CMAKE_SOURCE_DIR}/src/core/filters/VolatilityEstimator.cpp  # <-- ✅ Added VolatilityEstimator
)

# ──────────────────────────────────────────────────────────────────────────────
# Build executable
# ──────────────────────────────────────────────────────────────────────────────
add_executable(XAlgoNexus ${XALGO_SOURCES})

# Place the final .exe into build/bin/
set_target_properties(XAlgoNexus PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ──────────────────────────────────────────────────────────────────────────────
# Link in threading
# ──────────────────────────────────────────────────────────────────────────────
find_package(Threads REQUIRED)
target_link_libraries(XAlgoNexus PRIVATE Threads::Threads)

# ──────────────────────────────────────────────────────────────────────────────
# Copy data files into build/data/
# ──────────────────────────────────────────────────────────────────────────────
file(GLOB DATA_FILES "${CMAKE_SOURCE_DIR}/data/*.csv")
foreach(_f IN LISTS DATA_FILES)
  configure_file(${_f} ${CMAKE_BINARY_DIR}/data/ COPYONLY)
endforeach()

# ──────────────────────────────────────────────────────────────────────────────
message(STATUS "Project configured successfully.")
message(STATUS "Data directory: ${CMAKE_BINARY_DIR}/data")
