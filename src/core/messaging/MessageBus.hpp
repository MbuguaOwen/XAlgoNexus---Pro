// src/core/messaging/MessageBus.hpp
#pragma once

#include <chrono>
#include <variant>
#include <memory>
#include "Tick.hpp"
#include "moodycamel/concurrentqueue.h"

namespace XAlgo {

enum class MarketEventType { TICK, SPREAD, SIGNAL, EXECUTION };

struct MarketEvent {
    MarketEventType type;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::variant<Tick> payload;  // Just Tick for now

};

// Define an alias for the lock-free queue
using EventQueue = moodycamel::ConcurrentQueue<std::shared_ptr<MarketEvent>>;

} // namespace XAlgo
