// src/core/execution/ExecutionEngine.cpp

#include "core/execution/ExecutionEngine.hpp"
#include "core/signal/TradeSignal.hpp"
#include <iostream>
#include <chrono>

namespace XAlgo {

ExecutionEngine::ExecutionEngine(EventQueue& queue, RiskEngine* risk_engine)
    : queue_(queue),
      logger_(std::make_unique<TradeLogger>("live_trades.csv")),
      risk_engine_(risk_engine),
      running_(false)
{}

ExecutionEngine::~ExecutionEngine() {
    stop();
}

void ExecutionEngine::start() {
    running_ = true;
    thread_ = std::thread(&ExecutionEngine::run, this);
}

void ExecutionEngine::stop() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

void ExecutionEngine::run() {
    while (running_) {
        std::shared_ptr<MarketEvent> event;
        if (queue_.try_dequeue(event) && event && event->type == MarketEventType::SIGNAL) {
            const auto& signal = std::get<TradeSignal>(event->payload);

            const double fill_price = signal.price;
            const double size = risk_engine_->getLotSize();
            const double direction_multiplier = (signal.direction > 0) ? 1.0 : -1.0;
            const double pnl = direction_multiplier * size * 0.0001 * fill_price;

            risk_engine_->updateCapital(pnl);

            logger_->logTrade(
                signal.symbol,
                signal.direction > 0 ? "BUY" : "SELL",
                fill_price,
                size,
                pnl,
                risk_engine_->getCapital()
            );

            std::cout << "[EXECUTION] "
                      << (signal.direction > 0 ? "BUY" : "SELL") << " "
                      << signal.symbol << " @ " << fill_price
                      << " | PnL: " << pnl
                      << " | Capital: " << risk_engine_->getCapital()
                      << "\n";
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Light CPU load
        }
    }
}

} // namespace XAlgo
