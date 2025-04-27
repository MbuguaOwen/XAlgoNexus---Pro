#include "core/preprocess/TickPreprocessor.hpp"

namespace XAlgo {

TickPreprocessor::TickPreprocessor(MessageBus* bus)
: bus_(bus)
{}

void TickPreprocessor::run() {
    std::shared_ptr<MarketEvent> evt;
    while (bus_->poll(evt)) {
        // Optional: apply normalization, timestamp adjustment, or data enrichment here
        bus_->publish(evt);
    }
}

} // namespace XAlgo
