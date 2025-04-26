#pragma once

#include "core/messaging/Tick.hpp"
#include "core/messaging/MessageBus.hpp"
#include <optional>

namespace XAlgo {

class SpreadEngine {
public:
    SpreadEngine(EventQueue& signal_queue);

    void update(const ForexTick& tick);

private:
    void calculateSpread();

    std::optional<ForexTick> eurusd_;
    std::optional<ForexTick> gbpusd_;
    std::optional<ForexTick> eurgbp_;
    EventQueue& signal_queue_;
};

} // namespace XAlgo
