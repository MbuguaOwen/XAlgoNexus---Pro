#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <memory>
#include "core/messaging/MessageBus.hpp"
#include "core/messaging/Tick.hpp"
#include "core/ingestion/ForexReplayIngestor.hpp"

using namespace XAlgo;

int main() {
    EventQueue queue;
    std::atomic<bool> running{true};

    // Start ingestion for each forex pair
    ForexReplayIngestor eurusd_ingestor(queue, "../../ticks_eurusd.csv", "EUR/USD", ReplayMode::FastForward);
    ForexReplayIngestor gbpusd_ingestor(queue, "../../ticks_gbpusd.csv", "GBP/USD", ReplayMode::FastForward);
    ForexReplayIngestor eurgbp_ingestor(queue, "../../ticks_eurgbp.csv", "EUR/GBP", ReplayMode::FastForward);

    eurusd_ingestor.start();
    gbpusd_ingestor.start();
    eurgbp_ingestor.start();

    // Consumer thread
    std::thread consumer([&]() {
        size_t tick_count = 0;
        auto start_time = std::chrono::steady_clock::now();

        while (running || queue.size_approx() > 0) {
            std::shared_ptr<MarketEvent> event;
            if (queue.try_dequeue(event)) {
                if (event->type == MarketEventType::TICK) {
                    ForexTick tick = std::get<ForexTick>(event->payload);
                    std::cout << "Received Tick: " << tick.symbol << " Bid: " << tick.bid << " Ask: " << tick.ask << std::endl;
                    ++tick_count;
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            // Show ticks/second every 5 seconds
            if (tick_count > 0 && tick_count % 5000 == 0) {
                auto now = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
                if (duration > 0) {
                    std::cout << "Ticks/sec: " << tick_count / duration << std::endl;
                }
            }
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(10)); // Let it run
    eurusd_ingestor.stop();
    gbpusd_ingestor.stop();
    eurgbp_ingestor.stop();
    running = false;
    consumer.join();

    std::cout << "Forex Ingestion Complete." << std::endl;
}
