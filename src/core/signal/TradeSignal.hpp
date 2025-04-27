#pragma once
#include <chrono>

namespace XAlgo {

struct TradeSignal {
    std::chrono::system_clock::time_point timestamp;
    double spread;
    int direction; // +1 for buy synthetic (sell real), -1 for sell synthetic (buy real)
};

} // namespace XAlgo
