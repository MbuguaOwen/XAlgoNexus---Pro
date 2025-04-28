// src/core/ingest/ForexReplayIngestor.cpp

#include "core/ingest/ForexReplayIngestor.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

namespace XAlgo {

ForexReplayIngestor::ForexReplayIngestor(EventQueue& queue, const std::string& file_path, const std::string& symbol, ReplayMode mode)
    : queue_(queue), file_path_(file_path), symbol_(symbol), mode_(mode), running_(false)
{}

ForexReplayIngestor::~ForexReplayIngestor() {
    stop();
}

void ForexReplayIngestor::start() {
    if (running_) {
        std::cerr << "[WARN] ForexReplayIngestor is already running." << std::endl;
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
        std::cerr << "[ERROR] Cannot open historical Forex file: " << file_path_ << std::endl;
        running_ = false;
        return;
    }

    std::string line;
    std::getline(infile, line); // Skip header

    while (running_ && std::getline(infile, line)) {
        ForexTick tick = parseLine(line);

        if (!symbol_.empty()) {
            tick.symbol = symbol_;
        }

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

    std::cout << "[ForexReplayIngestor] Completed replay from: " << file_path_ << std::endl;
}

ForexTick ForexReplayIngestor::parseLine(const std::string& line) {
    std::istringstream ss(line);
    std::string ts, symbol, bid_str, ask_str;
    std::getline(ss, ts, ',');
    std::getline(ss, symbol, ',');
    std::getline(ss, bid_str, ',');
    std::getline(ss, ask_str, ',');

    ForexTick tick;
    tick.timestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(std::stoll(ts)));
    tick.symbol = symbol;
    tick.bid = std::stod(bid_str);
    tick.ask = std::stod(ask_str);
    tick.mid = (tick.bid + tick.ask) * 0.5;

    return tick;
}

} // namespace XAlgo
