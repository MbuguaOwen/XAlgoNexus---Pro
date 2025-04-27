#include "core/signal/SignalGenerator.hpp"
#include <iostream>

namespace XAlgo {

SignalGenerator::SignalGenerator(EventQueue& signal_queue, EventQueue& execution_queue)
    : signal_queue_(signal_queue),
      execution_queue_(execution_queue),
      running_(true)
{}

void SignalGenerator::run() {
    while (running_) {
        std::shared_ptr<MarketEvent> evt;
        if (signal_queue_.try_dequeue(evt) && evt->type == MarketEventType::SIGNAL) {
            const auto& signal = std::get<TradeSignal>(evt->payload);
            generateExecutionSignal(signal);
        }
    }
}

void SignalGenerator::generateExecutionSignal(const TradeSignal& signal) {
    auto execEvent = std::make_shared<MarketEvent>();
    execEvent->type = MarketEventType::EXECUTION;
    execEvent->timestamp = std::chrono::system_clock::now();
    execEvent->payload = signal;
    execution_queue_.enqueue(execEvent);

    std::cout << "[SIGNAL] Forwarded TradeSignal: " 
              << (signal.direction > 0 ? "BUY" : "SELL")
              << " | Spread=" << signal.spread 
              << " | Strength=" << signal.strength 
              << std::endl;
}

void SignalGenerator::stop() {
    running_ = false;
}

} // namespace XAlgo
