#pragma once

#include "core/messaging/MarketEvent.hpp"
#include <memory>
#include "moodycamel/concurrentqueue.h"

namespace XAlgo {

// Lock-free event queue for messages
using EventQueue = moodycamel::ConcurrentQueue<std::shared_ptr<MarketEvent>>;

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
