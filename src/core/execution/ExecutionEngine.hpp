#pragma once

#include "core/messaging/MessageBus.hpp"

namespace XAlgo {

class ExecutionEngine {
public:
    ExecutionEngine();
    void handleSignal(double spread);

    double getPnL() const { return pnl_; }

private:
    double pnl_;
};

} // namespace XAlgo

