#include "ExecutionEngine.hpp"
#include <iostream>
#include <thread>
#include <fstream>    // Needed for file writing
#include <iomanip>    // For controlling decimal formatting

namespace XAlgo {

ExecutionEngine::ExecutionEngine(
    double slippage_mean,
    double slippage_stddev,
    int latency_ms,
    double fill_probability,
    double max_drawdown
)
    : pnl_(0.0),
      slippage_mean_(slippage_mean),
      slippage_stddev_(slippage_stddev),
      latency_ms_(latency_ms),
      fill_probability_(fill_probability),
      max_drawdown_(max_drawdown),
      trading_active_(true),
      max_pnl_(0.0),
      rng_(std::random_device{}()),
      slippage_dist_(slippage_mean_, slippage_stddev_),
      random_fill_dist_(0.0, 1.0) {}

void ExecutionEngine::handleSignal(double spread) {
    if (!trading_active_) {
        std::cout << "[EXECUTION] Trading Halted - Risk Limit Exceeded." << std::endl;
        return;
    }

    // Simulate latency
    std::this_thread::sleep_for(std::chrono::milliseconds(latency_ms_));

    // Check fill probability
    double roll = random_fill_dist_(rng_);
    if (roll > fill_probability_) {
        std::cout << "[EXECUTION] Order Rejected (fill probability)" << std::endl;
        return;
    }

    // Apply slippage
    double slippage = slippage_dist_(rng_);
    double adjusted_spread = spread - slippage;

    // Simulate PnL
    double notional = 100000.0;
    double profit = adjusted_spread * notional;
    pnl_ += profit;

    trades_.push_back(TradeRecord{profit, profit >= 0.0});

    std::cout << "[EXECUTION] Spread Executed | "
              << "Spread (adj): " << adjusted_spread
              << " | Slippage: " << slippage
              << " | Profit: " << profit
              << " | Total PnL: " << pnl_
              << std::endl;

    if (pnl_ > max_pnl_) {
        max_pnl_ = pnl_;
    }

    if (pnl_ <= max_drawdown_) {
        trading_active_ = false;
        std::cout << "[RISK] Max Drawdown Hit. Trading Disabled." << std::endl;
    }
}

void ExecutionEngine::printReport() const {
    size_t total_trades = trades_.size();
    size_t winning_trades = 0;
    double total_profit = 0.0;
    double max_drawdown_recorded = 0.0;

    for (const auto& trade : trades_) {
        if (trade.win) ++winning_trades;
        total_profit += trade.profit;

        double drawdown = max_pnl_ - (max_pnl_ - trade.profit);
        if (drawdown > max_drawdown_recorded) {
            max_drawdown_recorded = drawdown;
        }
    }

    std::cout << "\n----- TRADING REPORT -----" << std::endl;
    std::cout << "Total Trades: " << total_trades << std::endl;
    std::cout << "Winning Trades: " << winning_trades << std::endl;
    std::cout << "Win Rate: " << (total_trades > 0 ? (double)winning_trades * 100.0 / total_trades : 0.0) << "%" << std::endl;
    std::cout << "Average Profit/Trade: " << (total_trades > 0 ? total_profit / total_trades : 0.0) << std::endl;
    std::cout << "Final PnL: " << pnl_ << std::endl;
    std::cout << "Max Drawdown Observed: " << max_drawdown_recorded << std::endl;
    std::cout << "---------------------------" << std::endl;
}

void ExecutionEngine::saveTradesToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Failed to open CSV file for writing: " << filename << std::endl;
        return;
    }

    // Write CSV header
    file << "TradeNumber,Profit,Win\n";

    size_t trade_number = 1;
    for (const auto& trade : trades_) {
        file << trade_number++ << ","
             << std::fixed << std::setprecision(5) << trade.profit << ","
             << (trade.win ? "1" : "0") << "\n";
    }

    file.close();
    std::cout << "[INFO] Trades exported to " << filename << std::endl;
}

} // namespace XAlgo
