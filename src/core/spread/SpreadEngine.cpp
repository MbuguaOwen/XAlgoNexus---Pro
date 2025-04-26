#include "SpreadEngine.hpp"
#include <iostream>

namespace XAlgo {

SpreadEngine::SpreadEngine(EventQueue& signal_queue)
    : signal_queue_(signal_queue) {}

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

        // Optionally: generate a SIGNAL event if spread exceeds threshold
        if (std::abs(spread) > 0.0001) { // threshold
            auto now = std::chrono::system_clock::now();
        }
    }
}

} // namespace XAlgo
