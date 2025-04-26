#pragma once

#include "core/messaging/MessageBus.hpp"
#include <random>
#include <vector>
#include <string>  // Needed for saveTradesToCSV

namespace XAlgo {

struct TradeRecord {
    double profit;
    bool win;
};

class ExecutionEngine {
public:
    ExecutionEngine(
        double slippage_mean = 0.0,
        double slippage_stddev = 0.00002,
        int latency_ms = 50,
        double fill_probability = 0.95,
        double max_drawdown = -1000.0
    );

    void handleSignal(double spread);
    double getPnL() const { return pnl_; }
    bool isTradingActive() const { return trading_active_; }
    void printReport() const;
    void saveTradesToCSV(const std::string& filename) const;  // ✅ New

private:
    double pnl_;
    double slippage_mean_;
    double slippage_stddev_;
    int latency_ms_;
    double fill_probability_;
    double max_drawdown_;
    bool trading_active_;

    std::vector<TradeRecord> trades_;
    double max_pnl_;  // For tracking max equity for drawdown calculations

    std::default_random_engine rng_;
    std::normal_distribution<double> slippage_dist_;
    std::uniform_real_distribution<double> random_fill_dist_;
};

} // namespace XAlgo
