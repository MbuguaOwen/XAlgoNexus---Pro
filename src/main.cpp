#define _CRT_SECURE_NO_WARNINGS
#include "core/messaging/MessageBus.hpp"
#include "core/ingest/HistoricalReplayIngestor.hpp"
#include "core/preprocess/TickPreprocessor.hpp"
#include "core/spread/SpreadEngine.hpp"
#include "core/signal/SignalGenerator.hpp"
#include "core/execution/ExecutionEngine.hpp"
#include "core/logger/TradeLogger.hpp"
#include "core/risk/RiskManagerThread.hpp"
#include <thread>
#include <memory>
#include <iostream>
#include <csignal>
#include <atomic>
#include <cstdlib>

std::atomic<bool> keepRunning(true);

void signalHandler(int signum) {
    std::cout << "\n[INFO] Interrupt signal (" << signum << ") received. Exiting gracefully...\n";
    keepRunning = false;
}

int main() {
    using namespace XAlgo;

    signal(SIGINT, signalHandler);

    const char* df = std::getenv("XALGO_DATA_FILE");
    const char* lf = std::getenv("XALGO_LIVE_TRADES_FILE");
    std::string dataFile = df ? df : "data/GBPUSD_ticks.csv";
    std::string liveTrades = lf ? lf : "live_trades.csv";

    std::cout << "[INFO] Data File: " << dataFile << "\n"
              << "[INFO] Live Trades Log: " << liveTrades << "\n";

    MessageBus bus;
    EventQueue signal_queue;
    EventQueue execution_queue;

    HistoricalReplayIngestor ingestor(&bus, dataFile);
    if (!ingestor.isFileOpen()) {
        std::cerr << "[ERROR] Unable to open data file.\n";
        return 1;
    }

    TickPreprocessor preprocessor(&bus);
    SpreadEngine spreadEngine(signal_queue, 0.0001);
    SignalGenerator signalGenerator(signal_queue, execution_queue);
    ExecutionEngine executionEngine;
    TradeLogger tradeLogger(liveTrades);
    RiskEngine riskEngine(-1000.0);  // Example risk limit
    RiskManagerThread riskMonitor(&riskEngine, keepRunning);

    executionEngine.attachRiskEngine(&riskEngine);

    // Launch threads
    std::thread threadRisk(&RiskManagerThread::run, &riskMonitor);
    std::thread threadIngest(&HistoricalReplayIngestor::run, &ingestor);
    std::thread threadPreprocess(&TickPreprocessor::run, &preprocessor);

    std::thread threadSpread([&]() {
        while (keepRunning) {
            std::shared_ptr<MarketEvent> evt;
            if (bus.poll(evt)) {
                if (evt->type == MarketEventType::FOREX_TICK) {
                    auto tick = std::get<ForexTick>(evt->payload);
                    spreadEngine.update(tick);
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        signalGenerator.stop();
    });

    std::thread threadSignalGen(&SignalGenerator::run, &signalGenerator);

    std::thread threadExec([&]() {
        while (keepRunning) {
            std::shared_ptr<MarketEvent> evt;
            if (execution_queue.try_dequeue(evt)) {
                if (evt->type == MarketEventType::EXECUTION) {
                    TradeSignal signal = std::get<TradeSignal>(evt->payload);
                    executionEngine.handleSignal(signal);
                    tradeLogger.logTrade(signal.spread_value, executionEngine.getPnL(), signal.side == TradeSignal::Side::BUY);
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    });

    // Wait for all threads
    threadIngest.join();
    threadPreprocess.join();
    threadSpread.join();
    threadSignalGen.join();
    threadExec.join();
    threadRisk.join();

    tradeLogger.flush();
    executionEngine.printReport();
    executionEngine.saveTradesToCSV("summary_trades.csv");

    std::cout << "[INFO] Trading system exited cleanly.\n";
    return 0;
}
