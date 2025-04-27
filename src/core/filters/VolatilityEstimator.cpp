// src/core/filters/VolatilityEstimator.cpp

#include "VolatilityEstimator.hpp"
#include <cmath>
#include <stdexcept>

namespace XAlgo {

VolatilityEstimator::VolatilityEstimator(std::size_t window_size)
  : window_size_(window_size), buffer_(window_size, 0.0),
    index_(0), count_(0), sum_(0.0), sum_sq_(0.0) {
    if (window_size_ == 0) {
        throw std::invalid_argument("VolatilityEstimator: window_size must be > 0");
    }
}

void VolatilityEstimator::addSample(double value) {
    if (count_ < window_size_) {
        buffer_[index_] = value;
        sum_    += value;
        sum_sq_ += value * value;
        ++count_;
    } else {
        double old = buffer_[index_];
        sum_    -= old;
        sum_sq_ -= old * old;
        buffer_[index_] = value;
        sum_    += value;
        sum_sq_ += value * value;
    }
    index_ = (index_ + 1) % window_size_;
}

double VolatilityEstimator::getVolatility() const {
    // Need at least 2 samples for an unbiased estimate
    if (count_ < 2) return 0.0;

    // Compute sample variance: (sum_sq - sum^2 / n) / (n - 1)
    double mean = sum_ / count_;
    double numerator = sum_sq_ - (sum_ * mean);
    double var = numerator / (count_ - 1);
    return var > 0.0 ? std::sqrt(var) : 0.0;
}

} // namespace XAlgo
