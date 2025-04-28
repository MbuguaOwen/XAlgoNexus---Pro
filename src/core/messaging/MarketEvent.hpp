// src/core/messaging/MarketEvent.hpp

#pragma once

/**
 * @file MarketEvent.hpp
 * @brief Defines the core MarketEvent data structure for XAlgo's event-driven architecture.
 */

#include "core/preprocess/ForexTick.hpp"
#include "core/signal/TradeSignal.hpp"
#include <chrono>
#include <variant>
#include <memory>

namespace XAlgo {

/**
 * @enum MarketEventType
 * @brief Enumerates the types of market events handled within XAlgo.
 */
enum class MarketEventType {
    FOREX_TICK,   ///< New Forex tick data.
    SPREAD,       ///< Spread calculation update.
    SIGNAL,       ///< Trading signal generated.
    EXECUTION     ///< Trade execution event.
};

/**
 * @struct MarketEvent
 * @brief Represents a timestamped event carrying diverse payloads in the system.
 */
struct MarketEvent {
    MarketEventType type; ///< Type of the event.
    std::chrono::system_clock::time_point timestamp; ///< Timestamp when the event occurred.
    std::variant<ForexTick, double, TradeSignal> payload; ///< Payload data (tick, spread value, or trade signal).
};

} // namespace XAlgo
