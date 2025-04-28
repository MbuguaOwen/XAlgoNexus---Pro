// src/core/risk/RiskManagerThread.hpp

#pragma once

/**
 * @file RiskManagerThread.hpp
 * @brief Background thread to monitor system-wide risk and trigger shutdowns if necessary.
 */

#include "core/risk/RiskEngine.hpp"
#include <atomic>
#include <thread>

namespace XAlgo {

/**
 * @class RiskManagerThread
 * @brief Periodically monitors account health and shuts down system if bankruptcy occurs.
 */
class RiskManagerThread {
public:
    /**
     * @brief Constructor.
     * @param risk_engine Pointer to the RiskEngine for monitoring.
     * @param running_flag Shared atomic flag controlling system status.
     */
    RiskManagerThread(RiskEngine* risk_engine, std::atomic<bool>& running_flag);

    /**
     * @brief Destructor.
     */
    ~RiskManagerThread();

    /**
     * @brief Start the risk monitoring thread.
     */
    void start();

    /**
     * @brief Stop the risk monitoring thread.
     */
    void stop();

private:
    void monitor(); ///< Internal monitoring loop.

    RiskEngine* risk_engine_; ///< Associated RiskEngine instance.
    std::atomic<bool>& running_; ///< Reference to global running control.
    std::thread thread_; ///< Monitoring background thread.
};

} // namespace XAlgo
