#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <memory>
#include "core/messaging/MessageBus.hpp"
#include "core/messaging/Tick.hpp"
#include "core/messaging/HistoricalReplayIngestor.hpp"

using namespace XAlgo;

int main() {
    EventQueue queue;
    std::atomic<bool> running{true};

    // HistoricalReplayIngestor reads real GBP/USD ticks
    HistoricalReplayIngestor ingestor(queue, "../../ticks_gbpusd.csv", ReplayMode::RealTime);
    ingestor.start();

    std::thread consumer([&]() {
        while (running || queue.size_approx() > 0) {
            std::shared_ptr<MarketEvent> event;
            if (queue.try_dequeue(event)) {
                if (event->type == MarketEventType::TICK) {
                    Tick tick = std::get<Tick>(event->payload);
                    std::cout << "Received Tick: " << tick.price << std::endl;
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(10));
    ingestor.stop();
    running = false;
    consumer.join();

    std::cout << "Historical Replay test complete." << std::endl;
}
