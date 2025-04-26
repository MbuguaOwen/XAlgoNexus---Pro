#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <atomic>
#include <memory>
#include "core/messaging/MessageBus.hpp"
#include "core/messaging/Tick.hpp"

namespace XAlgo {

enum class ReplayMode {
    RealTime,
    FastForward,
    SlowMotion
};

class ForexReplayIngestor {
public:
    ForexReplayIngestor(EventQueue& queue, const std::string& file_path, const std::string& symbol, ReplayMode mode = ReplayMode::RealTime);
    ~ForexReplayIngestor();

    void start();
    void stop();

private:
    void ingestLoop();
    EventQueue& queue_;
    std::string file_path_;
    std::string symbol_;
    ReplayMode mode_;
    std::atomic<bool> running_;
    std::thread ingest_thread_;
};

} // namespace XAlgo
