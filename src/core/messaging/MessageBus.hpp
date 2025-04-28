// src/core/messaging/MessageBus.hpp

#pragma once

/**
 * @file MessageBus.hpp
 * @brief Centralized message dispatch system for XAlgo modules.
 * 
 * Provides lock-free publishing and subscribing of market events between system components.
 */

#include "core/messaging/MarketEvent.hpp"
#include "moodycamel/concurrentqueue.h"
#include <functional>
#include <memory>

namespace XAlgo {

/**
 * @brief Alias for the lock-free concurrent event queue.
 */
using EventQueue = moodycamel::ConcurrentQueue<std::shared_ptr<MarketEvent>>;

/**
 * @class MessageBus
 * @brief Lightweight message-passing hub for market event distribution.
 * 
 * Allows modules to subscribe to market events and receive asynchronous notifications.
 * 
 * Note: Currently supports a single subscriber. Future extension can support multiple subscribers if needed.
 */
class MessageBus {
public:
    /**
     * @brief Construct a new MessageBus object.
     */
    MessageBus();

    /**
     * @brief Publish a new market event to subscribers.
     * 
     * @param event Shared pointer to the MarketEvent.
     */
    void publish(std::shared_ptr<MarketEvent> event);

    /**
     * @brief Register a subscriber callback for incoming events.
     * 
     * @tparam Func Callback function type.
     * @param callback The callback function that handles received events.
     */
    template<typename Func>
    void subscribe(Func&& callback) {
        callback_ = std::forward<Func>(callback);
    }

private:
    std::function<void(std::shared_ptr<MarketEvent>)> callback_; ///< Event callback handler.
};

} // namespace XAlgo
