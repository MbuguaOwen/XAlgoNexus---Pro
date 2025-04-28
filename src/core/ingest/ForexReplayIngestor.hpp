// src/core/ingest/ForexReplayIngestor.hpp

#pragma once

/**
 * @file ForexReplayIngestor.hpp
 * @brief Ingestor that replays Forex tick data into an event queue for backtesting.
 */

#include "core/messaging/MarketEvent.hpp"
#include "core/preprocess/ForexTick.hpp"
#include "core/common/ReplayMode.hpp"
#include "moodycamel/concurrentqueue.h"
#include <string>
#include <atomic>
#include <thread>
#include <memory>

namespace XAlgo {

/**
 * @class ForexReplayIngestor
 * @brief Ingests historical Forex tick data into a lock-free event queue.
 */
class ForexReplayIngestor {
public:
    /**
     * @brief Constructor.
     * @param queue Reference to the target event queue.
     * @param file_path Path to the historical data file.
     * @param symbol Target symbol override (optional).
     * @param mode Replay mode.
     */
    ForexReplayIngestor(EventQueue& queue, const std::string& file_path, const std::string& symbol, ReplayMode mode = ReplayMode::REALTIME);

    /**
     * @brief Destructor.
     */
    ~ForexReplayIngestor();

    /**
     * @brief Start the ingestion loop in a separate thread.
     */
    void start();

    /**
     * @brief Stop the ingestion loop.
     */
    void stop();

private:
    void ingestLoop();
    ForexTick parseLine(const std::string& line);

    EventQueue& queue_;           ///< Reference to shared event queue.
    std::string file_path_;       ///< Historical data file path.
    std::string symbol_;          ///< Optional symbol override.
    ReplayMode mode_;             ///< Replay behavior mode.
    std::atomic<bool> running_;   ///< Running status flag.
    std::thread ingest_thread_;   ///< Ingestion worker thread.
};

} // namespace XAlgo
