// main.cpp

#include "core/messaging/MessageBus.hpp"
#include "core/messaging/MarketEvent.hpp"
#include "core/ingest/HistoricalReplayIngestor.hpp"
#include "core/preprocess/TickPreprocessor.hpp"
#include "core/spread/SpreadEngine.hpp"
#include "core/signal/SignalGenerator.hpp"
#include "core/execution/ExecutionEngine.hpp"
#include "core/logger/TradeLogger.hpp"
#include "core/risk/RiskEngine.hpp"
#include "core/common/EventQueue.hpp"
#include "core/common/ReplayMode.hpp"

#include <thread>
#include <memory>
#include <iostream>
#include <csignal>
#include <atomic>

namespace XAlgo {

    std::atomic<bool> keepRunning(true);

    void signalHandler(int signum) {
        std::cout << "\n[INFO] Interrupt signal (" << signum << ") received. Exiting gracefully..." << std::endl;
        keepRunning = false;
    }

} // namespace XAlgo

int main() {
    using namespace XAlgo;

    // Register signal handler
    std::signal(SIGINT, signalHandler);

    // Instantiate core communication buses
    MessageBus bus;
    EventQueue spread_queue;
    EventQueue signal_queue;
    EventQueue execution_queue;

    // Instantiate modules
    std::string data_file = "data/GBPUSD_ticks.csv";
    HistoricalReplayIngestor ingestor(&bus, data_file, ReplayMode::REALTIME);

    TickPreprocessor preprocessor(bus, spread_queue);
    SpreadEngine spreadEngine(signal_queue);
    RiskEngine riskEngine(0.01, 10000.0); // 1% per trade risk, $10,000 starting capital
    ExecutionEngine executionEngine(execution_queue, riskEngine);
    SignalGenerator signalGenerator(signal_queue, execution_queue, riskEngine);

    // Setup Event Subscriptions
    bus.subscribe([&](std::shared_ptr<MarketEvent> event) {
        preprocessor.onMarketEvent(event);
    });

    spread_queue.enqueue_callback([&](std::shared_ptr<MarketEvent> event) {
        if (event->type == MarketEventType::FOREX_TICK) {
            ForexTick tick = std::get<ForexTick>(event->payload);
            spreadEngine.update(tick);
        }
    });

    // Start signal processing and execution engines
    signalGenerator.start();
    executionEngine.start();

    std::cout << "[INFO] Starting Historical Data Replay..." << std::endl;
    ingestor.run();  // Will stream market events into the bus

    // Main loop (watchdog for shutdown)
    while (keepRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Graceful Shutdown
    signalGenerator.stop();
    executionEngine.stop();

    std::cout << "[INFO] Shutdown complete." << std::endl;
    return 0;
}
