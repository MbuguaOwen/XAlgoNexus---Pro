// src/core/signal/SignalGenerator.cpp

#include "core/signal/SignalGenerator.hpp"
#include <iostream>

namespace XAlgo {

SignalGenerator::SignalGenerator(EventQueue& signal_queue, EventQueue& execution_queue, RiskEngine* risk_engine)
    : signal_queue_(signal_queue),
      execution_queue_(execution_queue),
      risk_engine_(risk_engine),
      running_(false)
{}

void SignalGenerator::start() {
    running_ = true;
    thread_ = std::thread(&SignalGenerator::run, this);
}

void SignalGenerator::stop() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

void SignalGenerator::run() {
    while (running_) {
        std::shared_ptr<MarketEvent> event;
        if (signal_queue_.try_dequeue(event) && event && event->type == MarketEventType::SPREAD) {
            if (risk_engine_->isBankrupt()) {
                std::cout << "[SIGNAL] Capital exhausted. Halting signal generation.\n";
                running_ = false;
                break;
            }

            double spread = std::get<double>(event->payload);
            TradeSignal signal;
            signal.symbol = "TRIANGULAR";
            signal.price = spread;
            signal.direction = decideDirection(spread);

            auto signal_event = std::make_shared<MarketEvent>(
                MarketEvent{MarketEventType::SIGNAL, std::chrono::system_clock::now(), signal}
            );
            execution_queue_.enqueue(std::move(signal_event));

            std::cout << "[SIGNAL] Spread: " << spread
                      << " | Direction: " << (signal.direction == 1 ? "BUY" : "SELL")
                      << "\n";
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

int SignalGenerator::decideDirection(double spread) {
    return (spread > 0.0) ? 1 : -1;
}

} // namespace XAlgo
