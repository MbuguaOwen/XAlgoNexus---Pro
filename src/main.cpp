#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <memory>
#include "core/messaging/MessageBus.hpp"
#include "core/messaging/Tick.hpp"
#include "core/ingestion/ForexReplayIngestor.hpp"
#include "core/spread/SpreadEngine.hpp"

using namespace XAlgo;

int main() {
    EventQueue tick_queue;   // Only one main event queue for ticks
    EventQueue signal_queue; // (optional) for later signal handling
    std::atomic<bool> running{true};

    SpreadEngine spread_engine(signal_queue);

    // Start ingestion for each forex pair
    ForexReplayIngestor eurusd_ingestor(tick_queue, "../../ticks_eurusd.csv", "EUR/USD", ReplayMode::FastForward);
    ForexReplayIngestor gbpusd_ingestor(tick_queue, "../../ticks_gbpusd.csv", "GBP/USD", ReplayMode::FastForward);
    ForexReplayIngestor eurgbp_ingestor(tick_queue, "../../ticks_eurgbp.csv", "EUR/GBP", ReplayMode::FastForward);

    eurusd_ingestor.start();
    gbpusd_ingestor.start();
    eurgbp_ingestor.start();

    // Consumer thread: read ticks and update Spread Engine
    std::thread consumer([&]() {
        size_t tick_count = 0;
        auto start_time = std::chrono::steady_clock::now();

        while (running || tick_queue.size_approx() > 0) {
            std::shared_ptr<MarketEvent> event;
            if (tick_queue.try_dequeue(event)) {
                if (event->type == MarketEventType::FOREX_TICK) {
                    ForexTick tick = std::get<ForexTick>(event->payload);

                    // Optional: Print tick
                    std::cout << "Received Tick: " << tick.symbol << " Bid: " << tick.bid << " Ask: " << tick.ask << std::endl;

                    // Important: Update SpreadEngine
                    spread_engine.update(tick);

                    ++tick_count;
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            // Show ticks/second every 5k ticks
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

    std::cout << "Forex Ingestion and Spread Calculation Complete." << std::endl;
}
