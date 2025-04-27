// Updated MarketEvent.hpp
#pragma once

#include <chrono>
#include <variant>
#include "core/preprocess/ForexTick.hpp"
#include "core/signal/TradeSignal.hpp"

namespace XAlgo {

enum class MarketEventType {
    TICK,
    FOREX_TICK,
    SPREAD,
    SIGNAL,
    EXECUTION
};

struct MarketEvent {
    MarketEventType type;
    std::chrono::system_clock::time_point timestamp;
    std::variant<ForexTick, double, TradeSignal> payload;
};

} // namespace XAlgo
