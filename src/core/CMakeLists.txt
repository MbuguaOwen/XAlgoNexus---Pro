# src/core/CMakeLists.txt

add_library(core
    messaging/MessageBus.cpp
    messaging/HistoricalReplayIngestor.cpp
    ingestion/ForexReplayIngestor.cpp
    messaging/Tick.cpp
    spread/SpreadEngine.cpp
    execution/ExecutionEngine.cpp
    spread/VolatilityEstimator.cpp
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core PUBLIC fmt)
