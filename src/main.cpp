// src/main.cpp

#include "core/messaging/MessageBus.hpp"
#include "core/messaging/MarketEvent.hpp"
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
#include <chrono>
#include <thread>

std::atomic<bool> keepRunning(true);

void signalHandler(int signum) {
    std::cout << "\n[INFO] Interrupt (" << signum << ") received. Exiting...\n";
    keepRunning = false;
}

int main() {
    using namespace XAlgo;

    // Setup signal handling for Ctrl+C
    std::signal(SIGINT, signalHandler);

    // Read environment variables
    const char* df = std::getenv("XALGO_DATA_FILE");
    const char* lf = std::getenv("XALGO_LIVE_TRADES_FILE");

    std::string dataFile    = df ? df : "data/GBPUSD_ticks.csv";
    std::string liveTrades  = lf ? lf : "live_trades.csv";

    std::cout << "[INFO] Using data file: " << dataFile
              << ", Logging to: " << liveTrades << "\n";

    // Instantiate buses and queues
    MessageBus      bus;
    EventQueue      signal_q, exec_q;

    // Instantiate components
    HistoricalReplayIngestor ingestor(&bus, dataFile);
    if (!ingestor.isFileOpen()) {
        std::cerr << "[ERROR] Cannot open data file. Exiting.\n";
        return 1;
    }

    TickPreprocessor   preproc(&bus);
    SpreadEngine       spreadEngine(signal_q, 0.0001);
    SignalGenerator    signalGen(signal_q, exec_q);
    ExecutionEngine    execEngine;
    TradeLogger        logger(liveTrades);
    RiskEngine         riskEngine(-1000.0);
    RiskManagerThread  riskMonitor(&riskEngine, keepRunning);

    execEngine.attachRiskEngine(&riskEngine);

    // Launch threads
    std::thread trisk(&RiskManagerThread::run, &riskMonitor);
    std::thread tingest(&HistoricalReplayIngestor::run, &ingestor);
    std::thread tpreproc(&TickPreprocessor::run, &preproc);

    std::thread tspread([&]() {
        while (keepRunning) {
            std::shared_ptr<MarketEvent> evt;
            if (bus.poll(evt)) {
                if (evt->type == MarketEventType::FOREX_TICK) {
                    auto fx = std::get<ForexTick>(evt->payload);
                    spreadEngine.update(fx);
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        signalGen.stop();
    });

    std::thread tsignalgen(&SignalGenerator::run, &signalGen);

    std::thread texecute([&]() {
        while (keepRunning) {
            std::shared_ptr<MarketEvent> evt;
            if (exec_q.try_dequeue(evt)) {
                if (evt->type == MarketEventType::EXECUTION) {
                    TradeSignal signal = std::get<TradeSignal>(evt->payload);
                    execEngine.handleSignal(signal);
                    logger.logTrade(signal.spread_value, execEngine.getPnL(), signal.side == TradeSignal::Side::BUY);
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    });

    // Wait for all threads
    tingest.join();
    tpreproc.join();
    tspread.join();
    tsignalgen.join();
    texecute.join();
    trisk.join();

    // Finalize
    logger.flush();
    execEngine.printReport();
    execEngine.saveTradesToCSV("summary_trades.csv");

    std::cout << "[INFO] System exited cleanly.\n";
    return 0;
}
