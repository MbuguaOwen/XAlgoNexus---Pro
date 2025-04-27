#pragma once

namespace XAlgo {

// 1D Simple Kalman Filter (price smoothing)
class KalmanFilter {
public:
    KalmanFilter(double process_variance, double measurement_variance);
    KalmanFilter() = delete;

    double update(double measurement);
    double getFilteredValue() const;

private:
    double process_variance_;
    double measurement_variance_;
    double estimate_;
    double error_estimate_;
    bool initialized_;
};

} // namespace XAlgo
