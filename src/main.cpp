#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <memory>
#include "core/messaging/MessageBus.hpp"
#include "core/messaging/Tick.hpp"
#include "core/ingestion/ForexReplayIngestor.hpp"
#include "core/spread/SpreadEngine.hpp"
#include "core/execution/ExecutionEngine.hpp"

using namespace XAlgo;

int main() {
    EventQueue tick_queue;   // Main tick queue
    EventQueue signal_queue; // Signal queue for spread signals
    std::atomic<bool> running{true};

    SpreadEngine spread_engine(signal_queue);
    ExecutionEngine execution_engine;

    // Start ingestion for each forex pair
    ForexReplayIngestor eurusd_ingestor(tick_queue, "../../ticks_eurusd.csv", "EUR/USD", ReplayMode::FastForward);
    ForexReplayIngestor gbpusd_ingestor(tick_queue, "../../ticks_gbpusd.csv", "GBP/USD", ReplayMode::FastForward);
    ForexReplayIngestor eurgbp_ingestor(tick_queue, "../../ticks_eurgbp.csv", "EUR/GBP", ReplayMode::FastForward);

    eurusd_ingestor.start();
    gbpusd_ingestor.start();
    eurgbp_ingestor.start();

    // Unified consumer thread: process Ticks + Spread Signals
    std::thread consumer([&]() {
        size_t tick_count = 0;
        auto start_time = std::chrono::steady_clock::now();

        while (running || tick_queue.size_approx() > 0 || signal_queue.size_approx() > 0) {
            std::shared_ptr<MarketEvent> event;

            // Process tick events
            if (tick_queue.try_dequeue(event)) {
                if (event->type == MarketEventType::FOREX_TICK) {
                    ForexTick tick = std::get<ForexTick>(event->payload);
                    spread_engine.update(tick);

                    ++tick_count;

                    // Optional: Print incoming ticks
                    std::cout << "Received Tick: " << tick.symbol << " Bid: " << tick.bid << " Ask: " << tick.ask << std::endl;
                }
            }

            // Process signal events
            if (signal_queue.try_dequeue(event)) {
                if (event->type == MarketEventType::SIGNAL) {
                    double spread = std::get<double>(event->payload);
                    execution_engine.handleSignal(spread);
                }
            }

            // Show ticks/sec every 5000 ticks
            if (tick_count > 0 && tick_count % 5000 == 0) {
                auto now = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
                if (duration > 0) {
                    std::cout << "Ticks/sec: " << tick_count / duration << std::endl;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    // Let it run for 10 seconds (example)
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Stop ingestion
    eurusd_ingestor.stop();
    gbpusd_ingestor.stop();
    eurgbp_ingestor.stop();
    running = false;

    // Wait for consumer to finish
    consumer.join();

    std::cout << "Forex Ingestion, Spread Calculation, and Execution Simulation Complete." << std::endl;
}
