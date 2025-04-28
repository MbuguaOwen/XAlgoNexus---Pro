// src/core/preprocess/TickPreprocessor.cpp

#include "core/preprocess/TickPreprocessor.hpp"

namespace XAlgo {

TickPreprocessor::TickPreprocessor(EventQueue& spread_queue)
    : spread_queue_(spread_queue)
{}

void TickPreprocessor::onMarketEvent(std::shared_ptr<MarketEvent> event) {
    if (!event) return;

    if (event->type == MarketEventType::FOREX_TICK) {
        spread_queue_.enqueue(std::move(event));
    }
}

} // namespace XAlgo
