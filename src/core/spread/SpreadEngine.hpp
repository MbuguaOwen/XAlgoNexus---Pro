// Updated SpreadEngine.hpp
#pragma once

#include "core/messaging/MarketEvent.hpp"
#include "core/preprocess/ForexTick.hpp"
#include "core/filters/VolatilityEstimator.hpp"
#include "moodycamel/concurrentqueue.h"
#include <memory>
#include <optional>

namespace XAlgo {

using EventQueue = moodycamel::ConcurrentQueue<std::shared_ptr<MarketEvent>>;

class SpreadEngine {
public:
    SpreadEngine(EventQueue& signal_queue, double initial_spread_threshold = 0.0001);

    void setSpreadThreshold(double threshold);
    void setVolatilityMultiplier(double multiplier);
    void update(const ForexTick& tick);

private:
    void calculateSpread();

    EventQueue& signal_queue_;
    double spread_threshold_;
    double volatility_multiplier_;

    VolatilityEstimator volatility_estimator_;

    std::optional<ForexTick> eurusd_;
    std::optional<ForexTick> gbpusd_;
    std::optional<ForexTick> eurgbp_;
};

} // namespace XAlgo
