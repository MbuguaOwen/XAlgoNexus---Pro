#include "core/signal/SignalGenerator.hpp"

namespace XAlgo {

SignalGenerator::SignalGenerator(EventQueue& in_queue, EventQueue& out_queue)
: in_queue_(in_queue), out_queue_(out_queue), stopRequested_(false)
{}

void SignalGenerator::run() {
    while (!stopRequested_) {
        std::shared_ptr<MarketEvent> evt;
        if (in_queue_.try_dequeue(evt) && evt->type == MarketEventType::SIGNAL) {
            // forward as an EXECUTION event
            auto execEvt = std::make_shared<MarketEvent>();
            execEvt->type      = MarketEventType::EXECUTION;
            execEvt->timestamp = evt->timestamp;
            execEvt->payload   = std::get<double>(evt->payload);
            out_queue_.enqueue(execEvt);
        }
    }
}

void SignalGenerator::stop() {
    stopRequested_ = true;
}

} // namespace XAlgo
