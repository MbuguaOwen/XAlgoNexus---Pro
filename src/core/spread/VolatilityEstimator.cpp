#include "VolatilityEstimator.hpp"
#include <numeric>
#include <cmath>

namespace XAlgo {

VolatilityEstimator::VolatilityEstimator(size_t window_size)
    : max_window_size_(window_size) {}

void VolatilityEstimator::addSample(double sample) {
    samples_.push_back(sample);
    if (samples_.size() > max_window_size_) {
        samples_.pop_front();
    }
}

double VolatilityEstimator::getVolatility() const {
    if (samples_.empty()) {
        return 0.0;
    }

    double mean = std::accumulate(samples_.begin(), samples_.end(), 0.0) / samples_.size();
    double variance = 0.0;

    for (auto s : samples_) {
        variance += (s - mean) * (s - mean);
    }
    variance /= samples_.size();
    return std::sqrt(variance);
}

} // namespace XAlgo
