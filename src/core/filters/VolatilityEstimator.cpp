// src/core/filters/VolatilityEstimator.cpp

#include "core/filters/VolatilityEstimator.hpp"
#include <cmath>    // For std::sqrt

namespace XAlgo {

VolatilityEstimator::VolatilityEstimator(std::size_t window_size)
    : window_size_(window_size)
{
    buffer_.resize(window_size_, 0.0);
}

void VolatilityEstimator::addSample(double value) {
    if (count_ < window_size_) {
        ++count_;
    } else {
        double old_value = buffer_[index_];
        sum_    -= old_value;
        sum_sq_ -= old_value * old_value;
    }

    buffer_[index_] = value;
    sum_    += value;
    sum_sq_ += value * value;

    index_ = (index_ + 1) % window_size_;
}

double VolatilityEstimator::getVolatility() const {
    if (count_ < 2) {
        return 0.0;
    }

    double mean = sum_ / static_cast<double>(count_);
    double mean_sq = mean * mean;
    double variance = (sum_sq_ / static_cast<double>(count_)) - mean_sq;

    // Defensive programming: avoid negative variance due to floating-point errors
    if (variance < 0.0) variance = 0.0;

    return std::sqrt(variance);
}

} // namespace XAlgo
