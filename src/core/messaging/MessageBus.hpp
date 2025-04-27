#pragma once

#include "core/messaging/MarketEvent.hpp"
#include "moodycamel/concurrentqueue.h"
#include <memory>

namespace XAlgo {

// Core event queue alias
using EventQueue = moodycamel::ConcurrentQueue<std::shared_ptr<MarketEvent>>;

// Simple in-process message bus
class MessageBus {
public:
    MessageBus() = default;

    void publish(std::shared_ptr<MarketEvent> event) {
        queue_.enqueue(std::move(event));
    }

    bool poll(std::shared_ptr<MarketEvent>& outEvent) {
        return queue_.try_dequeue(outEvent);
    }

private:
    EventQueue queue_;
};

} // namespace XAlgo
