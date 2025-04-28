// src/core/messaging/MessageBus.cpp

#include "core/messaging/MessageBus.hpp"

namespace XAlgo {

MessageBus::MessageBus() = default;

void MessageBus::publish(std::shared_ptr<MarketEvent> event) {
    if (callback_) {
        callback_(std::move(event));
    }
}

} // namespace XAlgo
