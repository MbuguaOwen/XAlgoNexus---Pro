#include "ExecutionEngine.hpp"
#include <iostream>

namespace XAlgo {

ExecutionEngine::ExecutionEngine()
    : pnl_(0.0) {}

void ExecutionEngine::handleSignal(double spread) {
    double notional = 100000.0; // Simulate trading 100,000 units (like standard FX lot)
    double profit = spread * notional; // Simplified calculation

    pnl_ += profit;

    std::cout << "[EXECUTION] Spread Arbitrage Executed | Spread: " 
              << spread << " | Profit: " << profit 
              << " | Total PnL: " << pnl_ << std::endl;
}

} // namespace XAlgo
