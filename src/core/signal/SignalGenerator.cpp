#include "core/signal/SignalGenerator.hpp"
#include <thread>
#include <variant>

namespace XAlgo {

SignalGenerator::SignalGenerator(EventQueue& signal_queue, EventQueue& execution_queue)
: signal_queue_(signal_queue), execution_queue_(execution_queue), running_(false)
{}

void SignalGenerator::run() {
    running_ = true;
    while (running_) {
        std::shared_ptr<MarketEvent> evt;
        if (signal_queue_.try_dequeue(evt)) {
            if (evt->type == MarketEventType::SIGNAL) {
                double spread = std::get<double>(evt->payload);
                int direction = decideDirection(spread);

                if (direction != 0) {
                    auto trade_signal = std::make_shared<MarketEvent>();
                    trade_signal->type = MarketEventType::EXECUTION;
                    trade_signal->timestamp = evt->timestamp;
                    trade_signal->payload = TradeSignal{
                        direction > 0 ? TradeSignal::Side::BUY : TradeSignal::Side::SELL,
                        spread,
                        evt->timestamp
                    };

                    execution_queue_.enqueue(trade_signal);
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void SignalGenerator::stop() {
    running_ = false;
}

int SignalGenerator::decideDirection(double spread) {
    if (spread > 0.0) {
        return -1; // SELL
    } else if (spread < 0.0) {
        return 1;  // BUY
    }
    return 0; // No action
}

} // namespace XAlgo
