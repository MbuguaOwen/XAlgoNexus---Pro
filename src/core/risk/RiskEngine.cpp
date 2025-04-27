#include "core/risk/RiskEngine.hpp"

namespace XAlgo {

RiskEngine::RiskEngine(double max_loss)
  : max_loss_(max_loss), cumulative_pnl_(0.0)
{}

void RiskEngine::updatePosition(double pnl_delta) {
    std::lock_guard lock(mtx_);
    cumulative_pnl_ += pnl_delta;
}

bool RiskEngine::isActive() const {
    std::lock_guard lock(mtx_);
    return cumulative_pnl_ > max_loss_; // Trading active if cumulative loss has not exceeded max_loss
}

void RiskEngine::reset() {
    std::lock_guard lock(mtx_);
    cumulative_pnl_ = 0.0;
}

} // namespace XAlgo
