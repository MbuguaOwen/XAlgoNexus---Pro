#pragma once

#include <chrono>

namespace XAlgo {

// Exponentially Weighted Moving Average Filter
class EWMAFilter {
public:
    explicit EWMAFilter(double alpha);
    EWMAFilter() = delete;

    double update(double price);
    double getSmoothedValue() const;

private:
    double alpha_;       // Smoothing factor
    double smoothed_;    // Current filtered value
    bool initialized_;
};

} // namespace XAlgo
