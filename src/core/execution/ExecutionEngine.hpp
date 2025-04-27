#pragma once

#include "core/messaging/MarketEvent.hpp"
#include "core/risk/RiskEngine.hpp"
#include <vector>
#include <random>
#include <string>

namespace XAlgo {

struct TradeRecord {
    double profit;
    bool win;
    double spread;
    double strength;
};

class ExecutionEngine {
public:
    ExecutionEngine();
    ExecutionEngine(double sl_mean, double sl_std, int latency, double fill_prob, double max_dd);

    void attachRiskEngine(RiskEngine* engine);
    void handleSignal(const TradeSignal& signal);

    double getPnL() const { return pnl_; }
    void printReport() const;
    void saveTradesToCSV(const std::string& filename) const;

private:
    double pnl_;
    double max_pnl_;
    double slippage_mean_;
    double slippage_stddev_;
    int latency_ms_;
    double fill_probability_;
    double max_drawdown_;
    bool trading_active_;

    RiskEngine* riskEngine_;

    std::mt19937 rng_;
    std::normal_distribution<double> slippage_dist_;
    std::uniform_real_distribution<double> random_fill_dist_;

    std::vector<TradeRecord> trades_;
};

} // namespace XAlgo
