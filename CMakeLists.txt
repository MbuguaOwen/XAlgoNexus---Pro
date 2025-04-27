cmake_minimum_required(VERSION 3.16)
project(XAlgoNexus LANGUAGES CXX)

# ──────────────────────────────────────────────────────────────────────────────
#  Basic settings
# ──────────────────────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type." FORCE)
endif()

# ──────────────────────────────────────────────────────────────────────────────
#  Warnings as errors
# ──────────────────────────────────────────────────────────────────────────────
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# ──────────────────────────────────────────────────────────────────────────────
#  Include paths
# ──────────────────────────────────────────────────────────────────────────────
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/core/risk
  ${CMAKE_SOURCE_DIR}/third_party
)

# ──────────────────────────────────────────────────────────────────────────────
#  Source files manually listed
# ──────────────────────────────────────────────────────────────────────────────
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
  ${CMAKE_SOURCE_DIR}/src/core/messaging/MessageBus.hpp
  ${CMAKE_SOURCE_DIR}/src/core/messaging/MarketEvent.hpp
  ${CMAKE_SOURCE_DIR}/src/core/risk/RiskManagerThread.cpp
  ${CMAKE_SOURCE_DIR}/VolatilityEstimator.cpp
  ${CMAKE_SOURCE_DIR}/src/core/ingest/ForexReplayIngestor.cpp
  ${CMAKE_SOURCE_DIR}/src/core/ingest/HistoricalReplayIngestor.cpp
  ${CMAKE_SOURCE_DIR}/src/core/spread/SpreadEngine.cpp
  ${CMAKE_SOURCE_DIR}/src/core/risk/RiskEngine.cpp


)

# ──────────────────────────────────────────────────────────────────────────────
#  Build executable
# ──────────────────────────────────────────────────────────────────────────────
add_executable(XAlgoNexus ${XALGO_SOURCES})

# ──────────────────────────────────────────────────────────────────────────────
#  Output settings
# ──────────────────────────────────────────────────────────────────────────────
set_target_properties(XAlgoNexus PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ──────────────────────────────────────────────────────────────────────────────
#  Link threading
# ──────────────────────────────────────────────────────────────────────────────
find_package(Threads REQUIRED)
target_link_libraries(XAlgoNexus PRIVATE Threads::Threads)

# ──────────────────────────────────────────────────────────────────────────────
#  (Docker Mode) — Data files are mounted externally, NOT copied into image
# ──────────────────────────────────────────────────────────────────────────────
# (No need to copy csvs into build tree anymore)

# ──────────────────────────────────────────────────────────────────────────────
#  Status Message
# ──────────────────────────────────────────────────────────────────────────────
message(STATUS "XAlgoNexus configured successfully [${CMAKE_BUILD_TYPE}]")
message(STATUS "NOTE: Data folder will be mounted externally in Docker.")
