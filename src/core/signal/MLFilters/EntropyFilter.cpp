#include "core/signal/MLFilters/EntropyFilter.hpp"
#include <cmath>
#include <numeric>

namespace XAlgo {

EntropyFilter::EntropyFilter(size_t window_size)
    : window_size_(window_size)
{}

void EntropyFilter::addSample(double value) {
    if (window_.size() >= window_size_) {
        window_.erase(window_.begin());
    }
    window_.push_back(value);
}

double EntropyFilter::computeEntropy() const {
    if (window_.empty()) return 0.0;

    // Normalize values
    double sum = std::accumulate(window_.begin(), window_.end(), 0.0);
    if (sum == 0.0) return 0.0;

    double entropy = 0.0;
    for (double val : window_) {
        double p = val / sum;
        if (p > 0) {
            entropy -= p * std::log2(p);
        }
    }
    return entropy;
}

} // namespace XAlgo
