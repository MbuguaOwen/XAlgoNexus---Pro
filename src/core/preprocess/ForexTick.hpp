#pragma once
#include <string>
#include <chrono>

namespace XAlgo {

struct ForexTick {
    std::chrono::milliseconds timestamp;
    std::string symbol;
    double bid;
    double ask;
};

} // namespace XAlgo
