// src/core/ingest/ForexReplayIngestor.cpp

#include "core/ingest/ForexReplayIngestor.hpp"
#include "core/preprocess/ForexTick.hpp"  // ✅ Added missing include
#include <iostream>
#include <chrono>
#include <thread>

namespace XAlgo {

ForexReplayIngestor::ForexReplayIngestor(EventQueue& queue, const std::string& file_path, const std::string& symbol, ReplayMode mode)
    : queue_(queue), file_path_(file_path), symbol_(symbol), mode_(mode), running_(false)
{}

ForexReplayIngestor::~ForexReplayIngestor() {
    stop();
}

void ForexReplayIngestor::start() {
    if (running_) {
        std::cerr << "[WARN] ForexReplayIngestor already running." << std::endl;
        return;
    }
    running_ = true;
    ingest_thread_ = std::thread(&ForexReplayIngestor::ingestLoop, this);
}

void ForexReplayIngestor::stop() {
    if (!running_) return;
    running_ = false;
    if (ingest_thread_.joinable()) {
        ingest_thread_.join();
    }
}

void ForexReplayIngestor::ingestLoop() {
    std::ifstream infile(file_path_);
    if (!infile.is_open()) {
        std::cerr << "[ERROR] Cannot open file: " << file_path_ << std::endl;
        running_ = false;
        return;
    }

    std::string line;
    std::getline(infile, line); // skip header

    while (running_ && std::getline(infile, line)) {
        ForexTick tick = parseLine(line);
        tick.symbol = symbol_;

        auto event = std::make_shared<MarketEvent>();
        event->type = MarketEventType::FOREX_TICK;
        event->timestamp = tick.timestamp;
        event->payload = tick;

        queue_.enqueue(event);

        switch (mode_) {
            case ReplayMode::RealTime:
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                break;
            case ReplayMode::FastForward:
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                break;
            case ReplayMode::SlowMotion:
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                break;
        }
    }

    std::cout << "[ForexReplayIngestor] Finished replay from " << file_path_ << std::endl;
}

ForexTick ForexReplayIngestor::parseLine(const std::string& line) {
    ForexTick tick;
    std::istringstream ss(line);
    std::string token;

    // Format: timestamp_in_ms,bid,ask
    std::getline(ss, token, ',');
    long long timestamp_ms = std::stoll(token);

    tick.timestamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(
        std::chrono::milliseconds(timestamp_ms)
    );

    std::getline(ss, token, ',');
    tick.bid = std::stod(token);

    std::getline(ss, token, ',');
    tick.ask = std::stod(token);

    tick.mid = (tick.bid + tick.ask) / 2.0;

    return tick;
}

} // namespace XAlgo
