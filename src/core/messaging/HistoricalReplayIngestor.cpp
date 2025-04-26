#include "HistoricalReplayIngestor.hpp"
#include <iomanip>
#include <sstream>
#include <ctime>

namespace XAlgo {

// Parse timestamp from "YYYY-MM-DDTHH:MM:SS" format
static std::chrono::system_clock::time_point parseTimestamp(const std::string& ts) {
    std::tm t = {};
    std::istringstream ss(ts.substr(0, 19)); // Ignore anything after seconds
    ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

    if (ss.fail()) {
        throw std::runtime_error("Failed to parse timestamp: " + ts);
    }

    return std::chrono::system_clock::from_time_t(std::mktime(&t));
}

HistoricalReplayIngestor::HistoricalReplayIngestor(EventQueue& queue, const std::string& file_path, ReplayMode mode)
    : queue_(queue), file_path_(file_path), mode_(mode), running_(false) {}

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
    std::chrono::system_clock::time_point last_tick_time;

    while (std::getline(file, line) && running_) {
        std::istringstream ss(line);
        std::string timestamp_str, price_str;
        if (std::getline(ss, timestamp_str, ',') && std::getline(ss, price_str)) {
            // Build a ForexTick instead of Tick
            ForexTick forex_tick;
            forex_tick.symbol = "EUR/USD"; // Hardcoded for now
            forex_tick.bid = std::stod(price_str);
            forex_tick.ask = forex_tick.bid; // Assume mid price (bid = ask) for this simplified replay
            forex_tick.timestamp = parseTimestamp(timestamp_str);

            auto event = std::make_shared<MarketEvent>(MarketEvent{
                MarketEventType::FOREX_TICK,
                forex_tick.timestamp,
                std::variant<ForexTick, double>{forex_tick} // ✅ Correct: use forex_tick
            });

            queue_.enqueue(event);

            if (last_tick_time.time_since_epoch().count() > 0) {
                auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(forex_tick.timestamp - last_tick_time);

                if (mode_ == ReplayMode::RealTime) {
                    if (delay.count() > 0) std::this_thread::sleep_for(delay);
                } else if (mode_ == ReplayMode::FastForward) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                } else if (mode_ == ReplayMode::SlowMotion) {
                    std::this_thread::sleep_for(delay * 2);
                }
            }
            last_tick_time = forex_tick.timestamp;
        }
    }
}

} // namespace XAlgo
