#pragma once

#include "core/risk/RiskEngine.hpp"
#include <atomic>

namespace XAlgo {

// Periodically checks RiskEngine and can stop trading by flipping a flag.
class RiskManagerThread {
public:
    RiskManagerThread(RiskEngine* engine, std::atomic<bool>& keepRunning);

    void run();

private:
    RiskEngine* engine_;
    std::atomic<bool>& keepRunning_;
};

} // namespace XAlgo
