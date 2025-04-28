// src/core/filters/VolatilityEstimator.hpp

#pragma once

/**
 * @file VolatilityEstimator.hpp
 * @brief Online rolling volatility estimator using Welford-like incremental update logic.
 */

#include <vector>
#include <cstddef>

namespace XAlgo {

/**
 * @class VolatilityEstimator
 * @brief Computes running volatility (standard deviation) over a sliding window.
 * 
 * Maintains a fixed-size rolling buffer to efficiently track volatility without full recomputation.
 */
class VolatilityEstimator {
public:
    /**
     * @brief Constructor.
     * @param window_size Number of most recent samples to include in the volatility calculation.
     */
    explicit VolatilityEstimator(std::size_t window_size);

    /**
     * @brief Add a new sample to the volatility calculation.
     * @param value New sample value.
     */
    void addSample(double value);

    /**
     * @brief Retrieve the current volatility estimate.
     * @return Estimated standard deviation of the rolling sample window.
     */
    [[nodiscard]] double getVolatility() const;

private:
    std::size_t window_size_; ///< Size of the rolling window.
    std::vector<double> buffer_; ///< Circular buffer holding recent samples.
    std::size_t index_ = 0; ///< Current insertion index in buffer.
    std::size_t count_ = 0; ///< Number of samples observed (up to window size).
    double sum_ = 0.0; ///< Sum of current window values.
    double sum_sq_ = 0.0; ///< Sum of squares of current window values.
};

} // namespace XAlgo
