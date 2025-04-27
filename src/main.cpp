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

std::atomic<bool> keepRunning(true);

void signalHandler(int signum) {
    std::cout << "\n[INFO] Interrupt (" << signum << ") received. Exiting gracefully...\n";
    keepRunning = false;
}

int main() {
    using namespace XAlgo;

    signal(SIGINT, signalHandler);

    const char* df = std::getenv("XALGO_DATA_FILE");
    const char* lf = std::getenv("XALGO_LIVE_TRADES_FILE");
    std::string dataFile = df ? df : "data/GBPUSD_ticks.csv";
    std::string liveTrades = lf ? lf : "live_trades.csv";

    std::cout << "[INFO] Data File: " << dataFile << "\n";
    std::cout << "[INFO] Live Trades Log: " << liveTrades << "\n";

    MessageBus bus;
    EventQueue signal_q, exec_q;

    HistoricalReplayIngestor ingestor(&bus, dataFile);
    if (!ingestor.isFileOpen()) return 1;

    TickPreprocessor preproc(&bus);
    SpreadEngine spread(signal_q, 0.0001);
    SignalGenerator signalGen(signal_q, exec_q);
    ExecutionEngine execEngine;
    TradeLogger logger(liveTrades);

    RiskEngine riskEngine(-1000.0, 5);
    RiskManagerThread riskThread(&riskEngine, keepRunning);

    execEngine.attachRiskEngine(&riskEngine);

    // Launch background threads
    std::thread tRisk(&RiskManagerThread::run, &riskThread);
    std::thread tIngest(&HistoricalReplayIngestor::run, &ingestor);
    std::thread tPreproc(&TickPreprocessor::run, &preproc);

    std::thread tSpread([&]() {
        while (keepRunning) {
            std::shared_ptr<MarketEvent> event;
            if (bus.poll(event) && event->type == MarketEventType::FOREX_TICK) {
                const auto& tick = std::get<ForexTick>(event->payload);
                spread.update(tick);
            }
        }
        signalGen.stop();
    });

    std::thread tSignalGen(&SignalGenerator::run, &signalGen);

    std::thread tExec([&]() {
        while (keepRunning) {
            std::shared_ptr<MarketEvent> event;
            if (exec_q.try_dequeue(event) && event->type == MarketEventType::EXECUTION) {
                const auto& signal = std::get<TradeSignal>(event->payload);
                execEngine.handleSignal(signal);
                logger.logTrade(signal.spread, execEngine.getPnL(), signal.spread > 0.0);
            }
        }
    });

    // Wait for all threads to finish
    tIngest.join();
    tPreproc.join();
    tSpread.join();
    tSignalGen.join();
    tExec.join();
    tRisk.join();

    logger.flush();
    execEngine.printReport();
    execEngine.saveTradesToCSV("summary_trades.csv");

    return 0;
}
