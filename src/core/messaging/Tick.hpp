#pragma once

#include <string>
#include <chrono>

namespace XAlgo {

// This struct is for Forex data (EUR/USD, GBP/USD, EUR/GBP)
struct ForexTick {
    std::string symbol; // Example: "EUR/USD", "GBP/USD"
    double bid = 0.0;    // Best bid price
    double ask = 0.0;    // Best ask price
    std::chrono::system_clock::time_point timestamp; // Exact moment of the tick
};
struct Tick {
    double price = 0.0;
    std::chrono::system_clock::time_point timestamp;
};

} // namespace XAlgo
