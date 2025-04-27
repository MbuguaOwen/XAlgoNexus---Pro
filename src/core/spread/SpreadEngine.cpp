#include "core/spread/SpreadEngine.hpp"
#include "core/signal/TradeSignal.hpp"
#include <iostream>
#include <cmath>

namespace XAlgo {

SpreadEngine::SpreadEngine(EventQueue& signal_queue, double initial_spread_threshold)
: signal_queue_(signal_queue)
, spread_threshold_(initial_spread_threshold)
, volatility_multiplier_(1.0)
, volatility_estimator_(50) // window size for volatility calculation
{}

void SpreadEngine::setSpreadThreshold(double threshold) {
    spread_threshold_ = threshold;
}

void SpreadEngine::setVolatilityMultiplier(double multiplier) {
    volatility_multiplier_ = multiplier;
}

void SpreadEngine::update(const ForexTick& tick) {
    if (tick.symbol == "GBP/USD") {
        gbpusd_ = tick;
    } else if (tick.symbol == "EUR/USD") {
        eurusd_ = tick;
    } else if (tick.symbol == "EUR/GBP") {
        eurgbp_ = tick;
    }

    // Only calculate spread if we have all three
    if (eurusd_ && gbpusd_ && eurgbp_) {
        calculateSpread();
    }
}

void SpreadEngine::calculateSpread() {
    double eurusd_mid = (eurusd_->bid + eurusd_->ask) / 2.0;
    double gbpusd_mid = (gbpusd_->bid + gbpusd_->ask) / 2.0;
    double eurgbp_mid = (eurgbp_->bid + eurgbp_->ask) / 2.0;

    // Synthetic EUR/GBP = EUR/USD ÷ GBP/USD
    double synthetic_eurgbp = eurusd_mid / gbpusd_mid;

    // Spread = real - synthetic
    double spread = eurgbp_mid - synthetic_eurgbp;

    volatility_estimator_.addSample(spread);
    double volatility = volatility_estimator_.getVolatility();
    double dynamic_threshold = spread_threshold_ + volatility_multiplier_ * volatility;

    if (std::abs(spread) > dynamic_threshold) {
        // Prepare signal
        TradeSignal signal;
        signal.timestamp = std::chrono::system_clock::now();
        signal.spread = spread;
        signal.direction = (spread > 0.0) ? -1 : 1; // if spread > 0, real EUR/GBP too rich, SELL it.

        auto event = std::make_shared<MarketEvent>();
        event->type = MarketEventType::SIGNAL;
        event->timestamp = signal.timestamp;
        event->payload = signal;
        signal_queue_.enqueue(event);

        std::cout << "[SPREAD] Signal generated! Spread=" << spread << ", Threshold=" << dynamic_threshold << "\n";
    }
}

} // namespace XAlgo
