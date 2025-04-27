// src/core/signal/SignalGenerator.hpp
#pragma once

#include "core/messaging/MarketEvent.hpp"
#include "moodycamel/concurrentqueue.h"
#include <memory>
#include <atomic>

namespace XAlgo {

    using EventQueue = moodycamel::ConcurrentQueue<std::shared_ptr<MarketEvent>>;

    class SignalGenerator {
    public:
        SignalGenerator(EventQueue& signal_queue, EventQueue& execution_queue);
        void run();
        void stop();

    private:
        EventQueue& signal_queue_;
        EventQueue& execution_queue_;
        std::atomic<bool> running_;
        int decideDirection(double spread);
    };

} // namespace XAlgo
