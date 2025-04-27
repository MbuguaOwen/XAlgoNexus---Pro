// src/core/ingest/HistoricalReplayIngestor.cpp

#include "../messaging/MessageBus.hpp"
#include "../messaging/MarketEvent.hpp"
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

class HistoricalReplayIngestor {
public:
    HistoricalReplayIngestor(MessageBus* bus, const std::string& dataFilePath)
        : bus_(bus), dataFilePath_(dataFilePath) {}

    void run() {
        std::ifstream file(dataFilePath_);
        std::string line;

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open historical data file");
        }

        while (std::getline(file, line)) {
            Tick tick = parseLine(line);

            MarketEvent event;
            event.timestamp = std::chrono::high_resolution_clock::now();
            event.type = MarketEvent::Type::TICK;
            event.payload = tick;

            bus_->publish(event);

            // Simulate real-time replay: sleep ~1ms between ticks
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

private:
    MessageBus* bus_;
    std::string dataFilePath_;

    Tick parseLine(const std::string& line) {
        Tick tick;
        std::istringstream ss(line);
        std::string token;
        
        // Example CSV format: timestamp,bid,ask
        std::getline(ss, token, ','); // timestamp (ignored here)
        std::getline(ss, token, ',');
        tick.bid = std::stod(token);
        std::getline(ss, token, ',');
        tick.ask = std::stod(token);

        tick.mid = (tick.bid + tick.ask) / 2.0;
        return tick;
    }
};
