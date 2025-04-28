// src/core/ingest/HistoricalReplayIngestor.hpp

#pragma once

/**
 * @file HistoricalReplayIngestor.hpp
 * @brief Module for replaying historical tick data into the MessageBus.
 */

#include "core/messaging/MessageBus.hpp"
#include "core/common/ReplayMode.hpp"
#include <fstream>
#include <string>
#include <memory>

namespace XAlgo {

/**
 * @class HistoricalReplayIngestor
 * @brief Streams historical tick data into the system, simulating real-time flow.
 */
class HistoricalReplayIngestor {
public:
    /**
     * @brief Constructor.
     * @param bus Pointer to the system MessageBus.
     * @param filename CSV file containing historical tick data.
     * @param mode Replay mode (RealTime, FastForward, SlowMotion).
     */
    HistoricalReplayIngestor(MessageBus* bus, const std::string& filename, ReplayMode mode);

    /**
     * @brief Destructor.
     */
    ~HistoricalReplayIngestor();

    /**
     * @brief Start replaying the historical data.
     */
    void run();

    /**
     * @brief Check if the data file is successfully open.
     * @return True if file is open, false otherwise.
     */
    bool isFileOpen() const;

private:
    /**
     * @brief Parse a CSV line into a MarketEvent.
     * @param line A single line from the input file.
     * @return Parsed MarketEvent.
     */
    MarketEvent parseLine(const std::string& line);

    MessageBus* bus_;            ///< Pointer to central message bus.
    std::string filename_;       ///< File path for historical data.
    std::ifstream infile_;       ///< Input file stream.
    ReplayMode mode_;            ///< Replay behavior mode.
};

} // namespace XAlgo
