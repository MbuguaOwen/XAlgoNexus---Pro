#pragma once

#include <chrono>
#include <string>

namespace XAlgo {

struct ForexTick {
    std::string symbol;
    double bid = 0.0;
    double ask = 0.0;
    double mid = 0.0;
    std::chrono::system_clock::time_point timestamp;
};

} // namespace XAlgo
