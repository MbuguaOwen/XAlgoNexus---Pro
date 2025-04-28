// src/core/signal/SignalGenerator.hpp

#pragma once

/**
 * @file SignalGenerator.hpp
 * @brief Generates trading signals based on spread events and enqueues them for execution.
 */

#include "core/messaging/MarketEvent.hpp"
#include "core/risk/RiskEngine.hpp"
#include "core/signal/TradeSignal.hpp"
#include "moodycamel/concurrentqueue.h"
#include <atomic>
#include <thread>
#include <memory>

namespace XAlgo {

/**
 * @class SignalGenerator
 * @brief Processes spread events and generates directional trade signals.
 */
class SignalGenerator {
public:
    /**
     * @brief Constructor.
     * @param signal_queue Reference to incoming spread events.
     * @param execution_queue Reference to outgoing trade signals.
     * @param risk_engine Pointer to the RiskEngine for bankruptcy checks.
     */
    SignalGenerator(EventQueue& signal_queue, EventQueue& execution_queue, RiskEngine* risk_engine);

    /**
     * @brief Start signal processing thread.
     */
    void start();

    /**
     * @brief Stop signal processing thread.
     */
    void stop();

private:
    void run();
    int decideDirection(double spread);

    EventQueue& signal_queue_;    ///< Queue receiving spread events.
    EventQueue& execution_queue_; ///< Queue sending trade signals to ExecutionEngine.
    RiskEngine* risk_engine_;     ///< Risk engine to verify available capital.

    std::atomic<bool> running_;   ///< Running status.
    std::thread thread_;          ///< Worker thread.
};

} // namespace XAlgo
