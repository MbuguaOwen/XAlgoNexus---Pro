// src/core/spread/SpreadEngine.cpp

#include "core/spread/SpreadEngine.hpp"
#include <iostream>
#include <cmath>

namespace XAlgo {

SpreadEngine::SpreadEngine(EventQueue& signal_queue)
    : signal_queue_(signal_queue),
      volatility_estimator_(100) // Window size for online volatility tracking
{}

void SpreadEngine::update(const ForexTick& tick) {
    if (tick.symbol == "EUR/USD") {
        eurusd_ = tick;
    } else if (tick.symbol == "GBP/USD") {
        gbpusd_ = tick;
    } else if (tick.symbol == "EUR/GBP") {
        eurgbp_ = tick;
    }

    if (eurusd_ && gbpusd_ && eurgbp_) {
        calculateSpread();
    }
}

void SpreadEngine::calculateSpread() {
    double implied_eurgbp = eurusd_->mid() / gbpusd_->mid();
    double actual_eurgbp = eurgbp_->mid();
    double spread = implied_eurgbp - actual_eurgbp;

    volatility_estimator_.addSample(spread);

    if (std::abs(spread) > spread_threshold_) {
        auto event = std::make_shared<MarketEvent>(
            MarketEvent{MarketEventType::SPREAD, std::chrono::system_clock::now(), spread}
        );
        signal_queue_.enqueue(std::move(event));

        std::cout << "[SPREAD] Spread detected: " << spread << "\n";
    }
}

} // namespace XAlgo
