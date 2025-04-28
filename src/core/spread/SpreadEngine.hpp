// src/core/spread/SpreadEngine.hpp

#pragma once

/**
 * @file SpreadEngine.hpp
 * @brief Calculates synthetic spreads for triangular arbitrage and generates spread events.
 */

#include "core/preprocess/ForexTick.hpp"
#include "core/filters/VolatilityEstimator.hpp"
#include "core/messaging/MarketEvent.hpp"
#include "moodycamel/concurrentqueue.h"
#include <optional>
#include <memory>

namespace XAlgo {

/**
 * @class SpreadEngine
 * @brief Computes the spread between synthetic and actual Forex pairs to identify arbitrage opportunities.
 */
class SpreadEngine {
public:
    /**
     * @brief Constructor.
     * @param signal_queue Reference to event queue for posting spread signals.
     */
    explicit SpreadEngine(EventQueue& signal_queue);

    /**
     * @brief Update SpreadEngine with a new Forex tick.
     * @param tick Incoming Forex tick.
     */
    void update(const ForexTick& tick);

private:
    void calculateSpread();

    EventQueue& signal_queue_; ///< Queue to send generated spread events.
    VolatilityEstimator volatility_estimator_; ///< Online volatility estimator for spread values.

    std::optional<ForexTick> eurusd_; ///< Latest EUR/USD tick.
    std::optional<ForexTick> gbpusd_; ///< Latest GBP/USD tick.
    std::optional<ForexTick> eurgbp_; ///< Latest EUR/GBP tick.

    const double spread_threshold_ = 0.0001; ///< Minimum spread threshold to trigger signals.
};

} // namespace XAlgo
