// src/core/ingest/ForexReplayIngestor.hpp
#pragma once

#include "core/messaging/MarketEvent.hpp"
#include "core/preprocess/ForexTick.hpp"
#include "moodycamel/concurrentqueue.h"
#include <string>
#include <fstream>
#include <sstream>
#include <atomic>
#include <thread>
#include <optional>

namespace XAlgo {

    using EventQueue = moodycamel::ConcurrentQueue<std::shared_ptr<MarketEvent>>;

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
        ForexTick parseLine(const std::string& line);

        EventQueue& queue_;
        std::string file_path_;
        std::string symbol_;
        ReplayMode mode_;
        std::atomic<bool> running_;
        std::thread ingest_thread_;
    };

} // namespace XAlgo
