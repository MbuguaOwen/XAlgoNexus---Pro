#pragma once

#include <chrono>

namespace XAlgo {

struct TradeSignal {
    enum class Side { BUY, SELL };

    Side side;
    double spread_value;
    std::chrono::system_clock::time_point timestamp;
};

} // namespace XAlgo
