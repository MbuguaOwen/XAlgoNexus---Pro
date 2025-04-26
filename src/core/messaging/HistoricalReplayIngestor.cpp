#include "HistoricalReplayIngestor.hpp"

namespace XAlgo {

HistoricalReplayIngestor::HistoricalReplayIngestor(EventQueue& queue, const std::string& file_path)
    : queue_(queue), file_path_(file_path), running_(false) {}

HistoricalReplayIngestor::~HistoricalReplayIngestor() {
    stop();
}

void HistoricalReplayIngestor::start() {
    running_ = true;
    ingest_thread_ = std::thread(&HistoricalReplayIngestor::ingestLoop, this);
}

void HistoricalReplayIngestor::stop() {
    running_ = false;
    if (ingest_thread_.joinable()) {
        ingest_thread_.join();
    }
}

void HistoricalReplayIngestor::ingestLoop() {
    std::ifstream file(file_path_);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open CSV file: " + file_path_);
    }

    std::string line;
    while (std::getline(file, line) && running_) {
        std::istringstream ss(line);
        std::string timestamp_str, price_str;
        if (std::getline(ss, timestamp_str, ',') && std::getline(ss, price_str)) {
            Tick tick;
            tick.timestamp = std::chrono::high_resolution_clock::now(); // Simulated live timestamp
            tick.price = std::stod(price_str);

            auto event = std::make_shared<MarketEvent>(
                MarketEvent{MarketEventType::TICK, tick.timestamp, tick}
            );

            queue_.enqueue(event);
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate pacing
        }
    }
}

} // namespace XAlgo
