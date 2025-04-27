#include "ForexReplayIngestor.hpp"
#include <iomanip>

namespace XAlgo {

ForexReplayIngestor::ForexReplayIngestor(EventQueue& queue, const std::string& file_path, const std::string& symbol, ReplayMode mode)
    : queue_(queue), file_path_(file_path), symbol_(symbol), mode_(mode), running_(false) {}

ForexReplayIngestor::~ForexReplayIngestor() {
    stop();
}

void ForexReplayIngestor::start() {
    running_ = true;
    ingest_thread_ = std::thread(&ForexReplayIngestor::ingestLoop, this);
}

void ForexReplayIngestor::stop() {
    running_ = false;
    if (ingest_thread_.joinable()) {
        ingest_thread_.join();
    }
}

static std::chrono::system_clock::time_point parseForexTimestamp(const std::string& ts) {
    std::tm t = {};
    std::istringstream ss(ts.substr(0, 23)); // e.g. "01.01.2010 23:59:49.604"
    ss >> std::get_time(&t, "%d.%m.%Y %H:%M:%S");
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse timestamp: " + ts);
    }
    return std::chrono::system_clock::from_time_t(std::mktime(&t));
}

void ForexReplayIngestor::ingestLoop() {
    std::ifstream file(file_path_);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open CSV file: " + file_path_);
    }

    std::string line;
    std::chrono::system_clock::time_point last_tick_time;

    while (std::getline(file, line) && running_) {
        std::istringstream ss(line);
        std::string timestamp_str, bid_str, ask_str;

        if (std::getline(ss, timestamp_str, ',') &&
            std::getline(ss, bid_str, ',') &&
            std::getline(ss, ask_str)) {

            ForexTick forex_tick;
            forex_tick.symbol = symbol_;
            forex_tick.timestamp = parseForexTimestamp(timestamp_str);
            forex_tick.bid = std::stod(bid_str);
            forex_tick.ask = std::stod(ask_str);

            auto event = std::make_shared<MarketEvent>(MarketEvent{
                MarketEventType::FOREX_TICK,
                forex_tick.timestamp,
                std::variant<ForexTick, double>{forex_tick} // ✅ Correctly wrap forex_tick
            });

            queue_.enqueue(event);

            if (last_tick_time.time_since_epoch().count() > 0) {
                auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(forex_tick.timestamp - last_tick_time);

                if (mode_ == ReplayMode::RealTime) {
                    if (delay.count() > 0) std::this_thread::sleep_for(delay);
                } else if (mode_ == ReplayMode::FastForward) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                } else if (mode_ == ReplayMode::SlowMotion) {
                    std::this_thread::sleep_for(delay * 2);
                }
            }

            last_tick_time = forex_tick.timestamp;
        }
    }
}

} // namespace XAlgo
