#pragma once

#include "core/messaging/Tick.hpp"
#include "core/messaging/MessageBus.hpp"

#include "VolatilityEstimator.hpp"  // ✅ Add this
#include <optional>

namespace XAlgo {

class SpreadEngine {
public:
    explicit SpreadEngine(EventQueue& signal_queue, double initial_spread_threshold = 0.0001);

    void setSpreadThreshold(double threshold);
    void update(const ForexTick& tick);

private:
    void calculateSpread();

    EventQueue& signal_queue_;
    double spread_threshold_;
    VolatilityEstimator volatility_estimator_;  // ✅ Use this new helper class
    double volatility_multiplier_ = 2.5;         // Scale the volatility to set the spread threshold
    std::optional<ForexTick> eurusd_, gbpusd_, eurgbp_;
};

} // namespace XAlgo
