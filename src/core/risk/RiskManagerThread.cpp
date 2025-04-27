#include "core/risk/RiskManagerThread.hpp"
#include <iostream>
#include <thread>
#include <chrono>

namespace XAlgo {

RiskManagerThread::RiskManagerThread(RiskEngine* engine, std::atomic<bool>& keepRunning)
  : engine_(engine), keepRunning_(keepRunning)
{}

void RiskManagerThread::run() {
    while (keepRunning_) {
        if (!engine_->isActive()) {
            std::cout << "[RISK] Risk limit breached! Halting trading.\n";
            keepRunning_ = false;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

} // namespace XAlgo
