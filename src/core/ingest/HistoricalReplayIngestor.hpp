// src/core/ingest/HistoricalReplayIngestor.hpp
#pragma once

#include "core/messaging/MessageBus.hpp"
#include "core/messaging/MarketEvent.hpp"
#include <string>
#include <fstream>

namespace XAlgo {

enum class ReplayMode { FAST, REALTIME };

class HistoricalReplayIngestor {
public:
    HistoricalReplayIngestor(MessageBus* bus,
                              const std::string& filename,
                              ReplayMode mode = ReplayMode::FAST);
    ~HistoricalReplayIngestor();

    bool isFileOpen() const;
    void run();

private:
    MarketEvent parseLine(const std::string& line);

    MessageBus*    bus_;
    std::string    filename_;
    std::ifstream  infile_;
    ReplayMode     mode_;
};

} // namespace XAlgo
