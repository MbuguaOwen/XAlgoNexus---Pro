// Updated SignalGenerator.hpp
#pragma once

#include "core/preprocess/ForexTick.hpp"
#include "core/messaging/MessageBus.hpp"
#include "core/signal/TradeSignal.hpp"
#include <memory>
#include <atomic>

namespace XAlgo {

class SignalGenerator {
public:
    SignalGenerator(EventQueue& signal_queue, EventQueue& execution_queue);
    void run();
    void stop();

private:
    int decideDirection(double spread);

    EventQueue& signal_queue_;
    EventQueue& execution_queue_;
    std::atomic<bool> running_;
};

} // namespace XAlgo

