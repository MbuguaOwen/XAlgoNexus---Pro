#pragma once

#include <string>
#include <chrono>

namespace XAlgo {

// Basic normalized Tick (for general use, e.g., equity, crypto)
struct Tick {
    double price = 0.0;
    std::chrono::system_clock::time_point timestamp;
};

// Forex specific Tick (bid/ask/mid model)
struct ForexTick {
    std::string symbol;
    double bid = 0.0;
    double ask = 0.0;
    double mid = 0.0;
    std::chrono::system_clock::time_point timestamp;
};

} // namespace XAlgo
