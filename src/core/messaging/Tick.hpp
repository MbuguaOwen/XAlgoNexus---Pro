// src/core/messaging/Tick.hpp
#pragma once

#include <chrono>

namespace XAlgo {
struct Tick {
    double price;
    std::chrono::high_resolution_clock::time_point timestamp;
};
}
