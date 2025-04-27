#include "core/signal/MLFilters/KalmanFilter.hpp"

namespace XAlgo {

KalmanFilter::KalmanFilter(double process_variance, double measurement_variance)
    : process_variance_(process_variance),
      measurement_variance_(measurement_variance),
      estimate_(0.0),
      error_estimate_(1.0),
      initialized_(false)
{}

double KalmanFilter::update(double measurement) {
    if (!initialized_) {
        estimate_ = measurement;
        initialized_ = true;
        return estimate_;
    }

    // Predict
    error_estimate_ += process_variance_;

    // Update
    double kalman_gain = error_estimate_ / (error_estimate_ + measurement_variance_);
    estimate_ = estimate_ + kalman_gain * (measurement - estimate_);
    error_estimate_ = (1.0 - kalman_gain) * error_estimate_;

    return estimate_;
}

double KalmanFilter::getFilteredValue() const {
    return estimate_;
}

} // namespace XAlgo
