#pragma once

#include "core/messaging/MessageBus.hpp"
#include "core/signal/TradeSignal.hpp"
#include <atomic>
#include <memory>

namespace XAlgo {

class SignalGenerator {
public:
    SignalGenerator(EventQueue& signal_queue, EventQueue& execution_queue);

    void run();
    void stop();

private:
    EventQueue& signal_queue_;
    EventQueue& execution_queue_;
    std::atomic<bool> running_;

    void generateExecutionSignal(const TradeSignal& signal);
};

} // namespace XAlgo
