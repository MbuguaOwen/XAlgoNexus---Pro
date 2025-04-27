#include "SpreadEngine.hpp"
#include <iostream>

namespace XAlgo {

SpreadEngine::SpreadEngine(EventQueue& signal_queue, double initial_spread_threshold)
    : signal_queue_(signal_queue),
      spread_threshold_(initial_spread_threshold),
      volatility_estimator_(500) {}

void SpreadEngine::setSpreadThreshold(double threshold) {
    spread_threshold_ = threshold;
}

void SpreadEngine::update(const ForexTick& tick) {
    if (tick.symbol == "EUR/USD") {
        eurusd_ = tick;
    } else if (tick.symbol == "GBP/USD") {
        gbpusd_ = tick;
    } else if (tick.symbol == "EUR/GBP") {
        eurgbp_ = tick;
    }

    calculateSpread();
}

void SpreadEngine::calculateSpread() {
    if (eurusd_ && gbpusd_ && eurgbp_) {
        double synthetic = gbpusd_->bid * eurgbp_->bid;
        double spread = eurusd_->bid - synthetic;

        std::cout << "[Spread] (EUR/USD) - (GBP/USD * EUR/GBP) = " << spread << std::endl;

        volatility_estimator_.addSample(spread);

        double volatility = volatility_estimator_.getVolatility();
        spread_threshold_ = volatility_multiplier_ * volatility;

        if (std::abs(spread) > spread_threshold_) {
            auto now = std::chrono::system_clock::now();

            auto event = std::make_shared<MarketEvent>(MarketEvent{
                MarketEventType::SIGNAL,
                now,
                std::variant<ForexTick, double>{spread}
            });

            signal_queue_.enqueue(event);

            std::cout << "[Signal] Spread signal generated: " << spread 
                      << " (Threshold: " << spread_threshold_ << ")" << std::endl;
        }
    }
}

} // namespace XAlgo
