// src/main.cpp

#include "core/messaging/MessageBus.hpp"
#include "core/ingest/HistoricalReplayIngestor.cpp"
#include "core/preprocess/TickPreprocessor.cpp"
#include "core/spread/SpreadEngine.cpp"
#include "core/signal/SignalGenerator.cpp"
#include "core/execution/ExecutionEngine.cpp"
#include "core/risk/RiskEngine.cpp"
#include "core/logger/TradeLogger.cpp"

#include <thread>

int main() {
    MessageBus bus;

    HistoricalReplayIngestor ingestor(&bus, "data/GBPUSD_ticks.csv");
    TickPreprocessor preprocessor(&bus);
    SpreadEngine spreadEngine(&bus);
    SignalGenerator signalGenerator(&bus);
    RiskEngine riskEngine(&bus);
    ExecutionEngine executionEngine(&bus);
    TradeLogger tradeLogger(&bus);

    // Launch threads for each component
    std::thread ingestThread(&HistoricalReplayIngestor::run, &ingestor);
    std::thread preprocessThread(&TickPreprocessor::run, &preprocessor);
    std::thread spreadThread(&SpreadEngine::run, &spreadEngine);
    std::thread signalThread(&SignalGenerator::run, &signalGenerator);
    std::thread riskThread(&RiskEngine::run, &riskEngine);
    std::thread executionThread(&ExecutionEngine::run, &executionEngine);
    std::thread loggerThread(&TradeLogger::run, &tradeLogger);

    // Join all threads
    ingestThread.join();
    preprocessThread.join();
    spreadThread.join();
    signalThread.join();
    riskThread.join();
    executionThread.join();
    loggerThread.join();

    return 0;
}
