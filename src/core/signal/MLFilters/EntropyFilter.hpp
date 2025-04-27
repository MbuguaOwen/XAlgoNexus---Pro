#pragma once

#include <vector>

namespace XAlgo {

// Simple Entropy-Based Filter (Volatility detection proxy)
class EntropyFilter {
public:
    explicit EntropyFilter(size_t window_size);
    EntropyFilter() = delete;

    void addSample(double value);
    double computeEntropy() const;

private:
    size_t window_size_;
    std::vector<double> window_;
};

} // namespace XAlgo
