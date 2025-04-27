#pragma once
#include <vector>

namespace XAlgo {

class VolatilityEstimator {
public:
  explicit VolatilityEstimator(std::size_t window_size);

  void addSample(double value);
  double getVolatility() const;

private:
  std::size_t window_size_;
  std::vector<double> buffer_;
  std::size_t index_;
  std::size_t count_;
  double sum_;
  double sum_sq_;
};

}  // namespace XAlgo
