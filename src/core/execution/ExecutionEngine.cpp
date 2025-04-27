#include "core/execution/ExecutionEngine.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <chrono>

namespace XAlgo {

ExecutionEngine::ExecutionEngine()
: ExecutionEngine(0.0001, 0.00005, 5, 0.9, -1000.0)
{}

ExecutionEngine::ExecutionEngine(double sl_mean, double sl_std, int latency, double fill_prob, double max_dd)
: pnl_(0.0),
  max_pnl_(0.0),
  min_pnl_(0.0),
  slippage_mean_(sl_mean),
  slippage_stddev_(sl_std),
  latency_ms_(latency),
  fill_probability_(fill_prob),
  max_drawdown_(max_dd),
  trading_active_(true),
  riskEngine_(nullptr),
  rng_(std::random_device{}()),
  slippage_dist_(sl_mean, sl_std),
  random_fill_dist_(0.0, 1.0)
{}

void ExecutionEngine::attachRiskEngine(RiskEngine* eng) {
    riskEngine_ = eng;
}

void ExecutionEngine::handleSignal(const TradeSignal& signal) {
    if (!trading_active_) {
        std::cout << "[EXEC] Trading halted by risk manager or manual stop.\n";
        return;
    }

    // Simulate network/API latency
    std::this_thread::sleep_for(std::chrono::milliseconds(latency_ms_));

    // Randomly determine if fill occurs
    if (random_fill_dist_(rng_) > fill_probability_) {
        std::cout << "[EXEC] Order rejected (no fill)\n";
        return;
    }

    // Apply slippage
    double slip = slippage_dist_(rng_);
    double adjusted_spread = (signal.spread_value > 0.0 ? signal.spread_value - slip
                                                        : signal.spread_value + slip);

    // Determine profit based on BUY/SELL
    double profit = 0.0;
    if (signal.side == TradeSignal::Side::BUY) {
        profit = adjusted_spread * 100000.0;
    } else { // SELL
        profit = -adjusted_spread * 100000.0;
    }

    pnl_ += profit;
    trades_.push_back({profit, profit >= 0.0});

    // Track PnL extrema
    if (pnl_ > max_pnl_) max_pnl_ = pnl_;
    if (pnl_ < min_pnl_) min_pnl_ = pnl_;

    if (pnl_ <= max_drawdown_) {
        std::cout << "[RISK] Max drawdown reached. Halting trading.\n";
        trading_active_ = false;
    }

    if (riskEngine_) {
        riskEngine_->updatePosition(profit);
    }
}

void ExecutionEngine::stopTrading() {
    trading_active_ = false;
}

void ExecutionEngine::printReport() const {
    size_t wins = 0;
    for (const auto& t : trades_) {
        if (t.win) ++wins;
    }

    double win_rate = (trades_.empty()) ? 0.0 : (static_cast<double>(wins) / trades_.size()) * 100.0;

    std::cout << "\n=== EXECUTION REPORT ===\n"
              << "Total Trades: " << trades_.size() << "\n"
              << "Win Rate: " << std::fixed << std::setprecision(2) << win_rate << "%\n"
              << "Final PnL: " << pnl_ << "\n"
              << "Max PnL: " << max_pnl_ << "\n"
              << "Max Drawdown: " << min_pnl_ << "\n"
              << "=========================\n";
}

void ExecutionEngine::saveTradesToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    file << "TradeNumber,Profit,Win\n";
    for (size_t i = 0; i < trades_.size(); ++i) {
        file << i + 1 << "," << trades_[i].profit << "," << (trades_[i].win ? 1 : 0) << "\n";
    }
}

} // namespace XAlgo
