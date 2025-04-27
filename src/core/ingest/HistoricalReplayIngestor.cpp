// src/core/ingest/HistoricalReplayIngestor.cpp

#include "core/ingest/HistoricalReplayIngestor.hpp"
#include "core/preprocess/ForexTick.hpp"  // ✅ Added missing include
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

namespace XAlgo {

HistoricalReplayIngestor::HistoricalReplayIngestor(MessageBus* bus,
                                                   const std::string& filename,
                                                   ReplayMode mode)
  : bus_(bus), filename_(filename), infile_(filename), mode_(mode)
{}

HistoricalReplayIngestor::~HistoricalReplayIngestor() {
    if (infile_.is_open()) {
        infile_.close();
    }
}

bool HistoricalReplayIngestor::isFileOpen() const {
    return infile_.is_open();
}

void HistoricalReplayIngestor::run() {
    std::string line;
    std::getline(infile_, line); // skip header

    while (std::getline(infile_, line) && bus_) {
        auto evt = parseLine(line);
        bus_->publish(std::make_shared<MarketEvent>(std::move(evt)));

        if (mode_ == ReplayMode::REALTIME) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

MarketEvent HistoricalReplayIngestor::parseLine(const std::string& line) {
    std::istringstream ss(line);
    std::string ts, pr;
    std::getline(ss, ts, ',');
    std::getline(ss, pr, ',');

    ForexTick tick;
    tick.timestamp = std::stoll(ts);
    tick.price     = std::stod(pr);

    MarketEvent e;
    e.type    = MarketEventType::TICK;
    e.payload = tick;
    return e;
}

}  // namespace XAlgo
