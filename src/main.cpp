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

    // ✅ Important: Correct relative path from build/Release
    HistoricalReplayIngestor ingestor(queue, "../../ticks.csv");
    ingestor.start();

    // ✅ Launch consumer thread
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

    // ✅ Let the ingestor run for enough time
    std::this_thread::sleep_for(std::chrono::seconds(5));

    ingestor.stop();
    running = false;
    consumer.join();

    std::cout << "Historical Replay test complete." << std::endl;
}
