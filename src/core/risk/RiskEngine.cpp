// src/core/risk/RiskEngine.cpp

#include "core/risk/RiskEngine.hpp"

namespace XAlgo {

RiskEngine::RiskEngine(double risk_percent, double initial_capital)
    : risk_percent_(risk_percent),
      capital_(initial_capital)
{}

double RiskEngine::getCapital() const {
    return capital_;
}

void RiskEngine::updateCapital(double pnl) {
    capital_ += pnl;
}

double RiskEngine::getLotSize() const {
    // Simplified pip-value model: 1000 USD per pip (e.g., EUR/USD)
    return (capital_ * risk_percent_) / 1000.0;
}

bool RiskEngine::isBankrupt() const {
    return capital_ <= 0.0;
}

} // namespace XAlgo
