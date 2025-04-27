
#include "core/preprocess/ForexTick.hpp"
#include "core/spread/SpreadEngine.hpp"
#include <iostream>
#include <cmath>

namespace XAlgo {

SpreadEngine::SpreadEngine(EventQueue& signal_queue, double initial_spread_threshold)
    : signal_queue_(signal_queue),
      spread_threshold_(initial_spread_threshold),
      volatility_multiplier_(2.0),   // Default: 2x standard deviation
      volatility_estimator_(500)      // Rolling window of 500 samples
{}

void SpreadEngine::setSpreadThreshold(double threshold) {
    spread_threshold_ = threshold;
}

void SpreadEngine::setVolatilityMultiplier(double multiplier) {
    volatility_multiplier_ = multiplier;
}

void SpreadEngine::update(const ForexTick& tick) {
    if (tick.symbol == "EUR/USD") {
        eurusd_ = tick;
    } else if (tick.symbol == "GBP/USD") {
        gbpusd_ = tick;
    } else if (tick.symbol == "EUR/GBP") {
        eurgbp_ = tick;
    }

    // Only calculate if all three are present
    if (eurusd_ && gbpusd_ && eurgbp_) {
        calculateSpread();
    }
}

void SpreadEngine::calculateSpread() {
    double eurusd_mid = (eurusd_->bid + eurusd_->ask) / 2.0;
    double gbpusd_mid = (gbpusd_->bid + gbpusd_->ask) / 2.0;
    double eurgbp_mid = (eurgbp_->bid + eurgbp_->ask) / 2.0;

    double synthetic_eur_usd = gbpusd_mid * eurgbp_mid;
    double spread = eurusd_mid - synthetic_eur_usd;

    // Feed into volatility estimator
    volatility_estimator_.addSample(spread);
    double volatility = volatility_estimator_.getVolatility();
    spread_threshold_ = volatility_multiplier_ * volatility;

    // Generate signal if spread exceeds threshold
    if (std::abs(spread) > spread_threshold_) {
        auto now = std::chrono::system_clock::now();

        auto event = std::make_shared<MarketEvent>();
        event->type = MarketEventType::SIGNAL;
        event->timestamp = now;
        event->payload = spread;

        signal_queue_.enqueue(event);

#ifdef DEBUG_SPREAD_ENGINE
        std::cout << "[SpreadEngine] Signal generated: Spread=" << spread
                  << ", Threshold=" << spread_threshold_ << std::endl;
#endif
    }
}

} // namespace XAlgo
