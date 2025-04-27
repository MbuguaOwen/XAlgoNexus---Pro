#include "core/execution/ExecutionEngine.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <random>

namespace XAlgo {

ExecutionEngine::ExecutionEngine()
    : ExecutionEngine(0.0001, 0.00005, 5, 0.9, -1000.0)
{}

ExecutionEngine::ExecutionEngine(double sl_mean, double sl_std, int latency, double fill_prob, double max_dd)
    : pnl_(0.0),
      max_pnl_(0.0),
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

void ExecutionEngine::attachRiskEngine(RiskEngine* engine) {
    riskEngine_ = engine;
}

void ExecutionEngine::handleSignal(const TradeSignal& signal) {
    if (!trading_active_) {
        std::cout << "[EXEC] Trading halted by risk engine.\n";
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(latency_ms_));

    if (random_fill_dist_(rng_) > fill_probability_) {
        std::cout << "[EXEC] Trade rejected due to fill probability.\n";
        return;
    }

    double slip = slippage_dist_(rng_);
    double effective_spread = signal.spread - slip;

    double profit = signal.direction * effective_spread * 100000.0;
    pnl_ += profit;
    trades_.push_back({ profit, profit >= 0.0, signal.spread, signal.strength });

    if (pnl_ > max_pnl_) max_pnl_ = pnl_;
    if (pnl_ <= max_drawdown_) trading_active_ = false;

    if (riskEngine_) {
        riskEngine_->updatePosition(profit);
    }

    std::cout << "[EXEC] Executed " << (signal.direction > 0 ? "BUY" : "SELL")
              << " | Spread=" << signal.spread
              << " | Strength=" << signal.strength
              << " | PnL=" << pnl_
              << std::endl;
}

void ExecutionEngine::printReport() const {
    size_t wins = 0;
    for (const auto& trade : trades_)
        if (trade.win) ++wins;

    std::cout << "\n=== EXECUTION REPORT ===\n"
              << "Total Trades: " << trades_.size()
              << ", Wins: " << wins
              << ", Win Rate: " << (trades_.empty() ? 0.0 : 100.0 * wins / trades_.size()) << "%\n"
              << "Final PnL: " << pnl_ << "\n";
}

void ExecutionEngine::saveTradesToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    file << "trade_number,profit,win,spread,strength\n";
    for (size_t i = 0; i < trades_.size(); ++i) {
        file << (i + 1) << ","
             << trades_[i].profit << ","
             << (trades_[i].win ? 1 : 0) << ","
             << trades_[i].spread << ","
             << trades_[i].strength << "\n";
    }
}

} // namespace XAlgo
