#pragma once

#include <deque>

namespace XAlgo {

class VolatilityEstimator {
public:
    VolatilityEstimator(size_t window_size = 500);

    void addSample(double sample);
    double getVolatility() const;

private:
    std::deque<double> samples_;
    size_t max_window_size_;
};

} // namespace XAlgo
