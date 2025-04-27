#pragma once

#include "core/messaging/MessageBus.hpp"
#include "core/messaging/MarketEvent.hpp"

namespace XAlgo {

class TickPreprocessor {
public:
    explicit TickPreprocessor(MessageBus* bus);
    TickPreprocessor() = delete; // prevent uninitialized bus

    void run();  // Main event loop

private:
    MessageBus* bus_;
};

} // namespace XAlgo
