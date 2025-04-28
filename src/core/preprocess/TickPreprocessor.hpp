// src/core/preprocess/TickPreprocessor.hpp

#pragma once

/**
 * @file TickPreprocessor.hpp
 * @brief Module for preprocessing raw tick events before spread calculation.
 */

#include "core/messaging/MarketEvent.hpp"
#include "core/preprocess/ForexTick.hpp"
#include "moodycamel/concurrentqueue.h"
#include <memory>

namespace XAlgo {

/**
 * @class TickPreprocessor
 * @brief Receives market events and forwards validated Forex ticks to the SpreadEngine.
 */
class TickPreprocessor {
public:
    /**
     * @brief Constructor.
     * @param spread_queue Reference to the queue where preprocessed events are forwarded.
     */
    explicit TickPreprocessor(EventQueue& spread_queue);

    /**
     * @brief Handles incoming market events.
     * @param event Incoming market event (shared pointer).
     */
    void onMarketEvent(std::shared_ptr<MarketEvent> event);

private:
    EventQueue& spread_queue_; ///< Reference to the spread calculation event queue.
};

} // namespace XAlgo
