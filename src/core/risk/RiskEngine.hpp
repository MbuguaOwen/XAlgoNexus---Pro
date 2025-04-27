#pragma once
#include <mutex>

namespace XAlgo {

class RiskEngine {
public:
    RiskEngine(double max_loss);

    void updatePosition(double pnl_delta);
    bool isActive() const;

    void reset(); // Optional reset for simulation/testing

private:
    double             max_loss_;
    double             cumulative_pnl_;
    mutable std::mutex mtx_;
};

} // namespace XAlgo
