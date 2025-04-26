#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <atomic>
#include <memory>
#include "core/messaging/MessageBus.hpp"
#include "Tick.hpp"

namespace XAlgo {

class HistoricalReplayIngestor {
public:
    HistoricalReplayIngestor(EventQueue& queue, const std::string& file_path);
    ~HistoricalReplayIngestor();

    void start();
    void stop();

private:
    void ingestLoop();

    EventQueue& queue_;
    std::string file_path_;
    std::atomic<bool> running_;
    std::thread ingest_thread_;
};

} // namespace XAlgo
