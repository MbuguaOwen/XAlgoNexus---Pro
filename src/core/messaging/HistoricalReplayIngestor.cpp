#include "HistoricalReplayIngestor.hpp"

namespace XAlgo {

static std::chrono::high_resolution_clock::time_point parseTimestamp(const std::string& ts) {
    // Dummy parser: just use now
    return std::chrono::high_resolution_clock::now();
}

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
    std::chrono::high_resolution_clock::time_point last_tick_time;

    while (std::getline(file, line) && running_) {
        std::istringstream ss(line);
        std::string timestamp_str, price_str;
        if (std::getline(ss, timestamp_str, ',') && std::getline(ss, price_str)) {
            Tick tick;
            tick.timestamp = parseTimestamp(timestamp_str); // Parse timestamp properly later
            tick.price = std::stod(price_str);

            auto now = std::chrono::high_resolution_clock::now();
            if (last_tick_time.time_since_epoch().count() > 0) {
                auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(tick.timestamp - last_tick_time);
                if (delay.count() > 0) {
                    std::this_thread::sleep_for(delay);
                }
            }

            last_tick_time = tick.timestamp;

            auto event = std::make_shared<MarketEvent>(
                MarketEvent{MarketEventType::TICK, tick.timestamp, tick}
            );

            queue_.enqueue(event);
        }
    }
}

} // namespace XAlgo
