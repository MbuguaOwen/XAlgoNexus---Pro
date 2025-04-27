#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <atomic>
#include <memory>
#include "MessageBus.hpp"
#include "Tick.hpp"

namespace XAlgo {

enum class ReplayMode {
    RealTime,
    FastForward,
    SlowMotion
};

class HistoricalReplayIngestor {
public:
    HistoricalReplayIngestor(EventQueue& queue, const std::string& file_path, ReplayMode mode = ReplayMode::RealTime);
    ~HistoricalReplayIngestor();

    void start();
    void stop();

private:
    void ingestLoop();
    EventQueue& queue_;
    std::string file_path_;
    ReplayMode mode_;
    std::atomic<bool> running_;
    std::thread ingest_thread_;
};

} // namespace XAlgo
