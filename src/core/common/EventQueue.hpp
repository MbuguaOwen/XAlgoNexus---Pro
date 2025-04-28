// src/core/common/EventQueue.hpp

#pragma once

/**
 * @file EventQueue.hpp
 * @brief Provides a lock-free event queue alias used for passing MarketEvents between system modules.
 */

#include "core/messaging/MarketEvent.hpp"
#include "moodycamel/concurrentqueue.h"
#include <memory>

namespace XAlgo {

/**
 * @typedef EventQueue
 * @brief Lock-free concurrent queue of MarketEvent shared pointers.
 * 
 * Used for high-speed, thread-safe communication between different components (e.g., Preprocessor, SpreadEngine, ExecutionEngine).
 */
using EventQueue = moodycamel::ConcurrentQueue<std::shared_ptr<MarketEvent>>;

} // namespace XAlgo
