// src/core/execution/ExecutionEngine.hpp

#pragma once

/**
 * @file ExecutionEngine.hpp
 * @brief Module responsible for consuming trade signals and executing simulated trades.
 */

#include "core/messaging/MarketEvent.hpp"
#include "core/logger/TradeLogger.hpp"
#include "core/risk/RiskEngine.hpp"
#include <memory>
#include <atomic>
#include <thread>

namespace XAlgo {

/**
 * @class ExecutionEngine
 * @brief Executes trades based on incoming trade signals, manages PnL, and logs execution details.
 */
class ExecutionEngine {
public:
    /**
     * @brief Constructor.
     * @param queue Event queue receiving trade signals.
     * @param risk_engine Pointer to the associated RiskEngine instance.
     */
    ExecutionEngine(EventQueue& queue, RiskEngine* risk_engine);

    /**
     * @brief Destructor (ensures clean shutdown).
     */
    ~ExecutionEngine();

    /**
     * @brief Start the execution engine loop in a separate thread.
     */
    void start();

    /**
     * @brief Stop the execution engine loop and join the thread.
     */
    void stop();

private:
    void run(); ///< Internal event-processing loop.

    EventQueue& queue_; ///< Event queue with trade signals.
    std::unique_ptr<TradeLogger> logger_; ///< Responsible for persisting executed trades.
    RiskEngine* risk_engine_; ///< Manages capital updates and risk constraints.
    std::atomic<bool> running_; ///< Running flag for the thread loop.
    std::thread thread_; ///< Background worker thread.
};

} // namespace XAlgo
