#pragma once

#include <string>
#include <chrono>

namespace XAlgo {

struct ForexTick {
    std::string symbol; // e.g., "EUR/USD"
    double bid = 0.0;
    double ask = 0.0;
    std::chrono::system_clock::time_point timestamp;
};

} // namespace XAlgo
