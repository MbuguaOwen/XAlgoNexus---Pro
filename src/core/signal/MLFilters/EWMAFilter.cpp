#include "core/signal/MLFilters/EWMAFilter.hpp"

namespace XAlgo {

EWMAFilter::EWMAFilter(double alpha)
    : alpha_(alpha), smoothed_(0.0), initialized_(false)
{}

double EWMAFilter::update(double price) {
    if (!initialized_) {
        smoothed_ = price;
        initialized_ = true;
    } else {
        smoothed_ = alpha_ * price + (1.0 - alpha_) * smoothed_;
    }
    return smoothed_;
}

double EWMAFilter::getSmoothedValue() const {
    return smoothed_;
}

} // namespace XAlgo
