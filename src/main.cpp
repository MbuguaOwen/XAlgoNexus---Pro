#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <memory>
#include <iomanip>
#include <sstream>
#include <ctime>

#include "core/messaging/MessageBus.hpp"
#include "core/messaging/Tick.hpp"
#include "core/ingestion/ForexReplayIngestor.hpp"
#include "core/spread/SpreadEngine.hpp"
#include "core/execution/ExecutionEngine.hpp"

using namespace XAlgo;

// Utility function to generate a timestamped filename
std::string generateTimestampedFilename(const std::string& prefix, const std::string& extension) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::tm local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &now_time);
#else
    localtime_r(&now_time, &local_tm);
#endif

    std::ostringstream oss;
    oss << prefix << "_" 
        << std::put_time(&local_tm, "%Y-%m-%d_%H-%M-%S") 
        << extension;
    return oss.str();
}

int main() {
    EventQueue tick_queue;
    EventQueue signal_queue;
    std::atomic<bool> running{true};

    SpreadEngine spread_engine(signal_queue);
    ExecutionEngine execution_engine(
        0.0,        // slippage_mean
        0.00002,    // slippage_stddev
        50,         // latency_ms
        0.95,       // fill_probability
        -1000.0     // max_drawdown
    );

    // Start ingestion
    ForexReplayIngestor eurusd_ingestor(tick_queue, "../../ticks_eurusd.csv", "EUR/USD", ReplayMode::FastForward);
    ForexReplayIngestor gbpusd_ingestor(tick_queue, "../../ticks_gbpusd.csv", "GBP/USD", ReplayMode::FastForward);
    ForexReplayIngestor eurgbp_ingestor(tick_queue, "../../ticks_eurgbp.csv", "EUR/GBP", ReplayMode::FastForward);

    eurusd_ingestor.start();
    gbpusd_ingestor.start();
    eurgbp_ingestor.start();

    // Unified consumer thread
    std::thread consumer([&]() {
        size_t tick_count = 0;
        auto start_time = std::chrono::steady_clock::now();

        while (running || tick_queue.size_approx() > 0 || signal_queue.size_approx() > 0) {
            std::shared_ptr<MarketEvent> event;

            // Process ticks
            if (tick_queue.try_dequeue(event)) {
                if (event->type == MarketEventType::FOREX_TICK) {
                    ForexTick tick = std::get<ForexTick>(event->payload);
                    spread_engine.update(tick);

                    ++tick_count;

                    // Optional: print tick
                    std::cout << "Received Tick: " << tick.symbol 
                              << " Bid: " << tick.bid 
                              << " Ask: " << tick.ask 
                              << std::endl;
                }
            }

            // Process signals
            if (signal_queue.try_dequeue(event)) {
                if (event->type == MarketEventType::SIGNAL) {
                    double spread = std::get<double>(event->payload);
                    execution_engine.handleSignal(spread); 
                }
            }

            // Print ticks/sec
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

    std::this_thread::sleep_for(std::chrono::seconds(10));

    eurusd_ingestor.stop();
    gbpusd_ingestor.stop();
    eurgbp_ingestor.stop();
    running = false;

    consumer.join();

    // ✅ Print Trading Report
    execution_engine.printReport();

    // ✅ Save Trade History to CSV (timestamped)
    std::string filename = generateTimestampedFilename("trades", ".csv");
    execution_engine.saveTradesToCSV(filename);

    std::cout << "Forex Ingestion, Spread Calculation, Execution Simulation, and Trade Export Complete." << std::endl;
}
