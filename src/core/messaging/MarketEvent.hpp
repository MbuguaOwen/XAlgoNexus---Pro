#pragma once

#include <chrono>
#include <variant>
#include <memory>
#include "core/preprocess/Tick.hpp"    // Correct unified Tick structure
#include "core/signal/SignalGenerator.hpp" // TradeSignal struct for execution events

namespace XAlgo {

enum class MarketEventType {
    TICK,           // General tick (any asset)
    FOREX_TICK,     // Forex-specific tick
    SPREAD,         // Calculated synthetic spread
    SIGNAL,         // Trading signal (buy/sell decision)
    EXECUTION       // Executed trade event
};

// Unified message event
struct MarketEvent {
    MarketEventType type;
    std::chrono::system_clock::time_point timestamp;

    // Variant can hold different payload types depending on event
    std::variant<ForexTick, double, TradeSignal> payload;
};

} // namespace XAlgo
