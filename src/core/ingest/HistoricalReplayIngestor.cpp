// src/core/ingest/HistoricalReplayIngestor.cpp

#include "core/ingest/HistoricalReplayIngestor.hpp"
#include "core/preprocess/ForexTick.hpp"
#include <sstream>
#include <thread>
#include <chrono>
#include <iostream>

namespace XAlgo {

HistoricalReplayIngestor::HistoricalReplayIngestor(MessageBus* bus, const std::string& filename, ReplayMode mode)
    : bus_(bus), filename_(filename), mode_(mode), infile_(filename)
{
    if (!infile_.is_open()) {
        std::cerr << "[ERROR] Failed to open historical data file: " << filename_ << std::endl;
    }
}

HistoricalReplayIngestor::~HistoricalReplayIngestor() {
    if (infile_.is_open()) {
        infile_.close();
    }
}

bool HistoricalReplayIngestor::isFileOpen() const {
    return infile_.is_open();
}

void HistoricalReplayIngestor::run() {
    if (!infile_.is_open()) {
        std::cerr << "[ERROR] File not open, cannot run replay." << std::endl;
        return;
    }

    std::string line;
    std::getline(infile_, line); // Skip CSV header

    while (std::getline(infile_, line)) {
        MarketEvent event = parseLine(line);
        bus_->publish(std::make_shared<MarketEvent>(std::move(event)));

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

    std::cout << "[HistoricalReplayIngestor] Completed data replay from file: " << filename_ << std::endl;
}

MarketEvent HistoricalReplayIngestor::parseLine(const std::string& line) {
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

    MarketEvent event;
    event.type = MarketEventType::FOREX_TICK;
    event.timestamp = tick.timestamp;
    event.payload = tick;

    return event;
}

} // namespace XAlgo
