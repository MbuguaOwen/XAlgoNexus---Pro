#pragma once

#include <chrono>
#include <variant>
#include <memory>
#include "Tick.hpp"
#include "moodycamel/concurrentqueue.h"

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
        std::chrono::system_clock::time_point timestamp;  // <<<<<< CHANGE THIS LINE
        std::variant<ForexTick, double> payload;
    };
    
    

// Define an alias for the lock-free queue
using EventQueue = moodycamel::ConcurrentQueue<std::shared_ptr<MarketEvent>>;

} // namespace XAlgo
