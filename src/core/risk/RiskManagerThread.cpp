// src/core/risk/RiskManagerThread.cpp

#include "core/risk/RiskManagerThread.hpp"
#include <iostream>
#include <chrono>

namespace XAlgo {

RiskManagerThread::RiskManagerThread(RiskEngine* risk_engine, std::atomic<bool>& running_flag)
    : risk_engine_(risk_engine),
      running_(running_flag)
{}

RiskManagerThread::~RiskManagerThread() {
    stop();
}

void RiskManagerThread::start() {
    thread_ = std::thread(&RiskManagerThread::monitor, this);
}

void RiskManagerThread::stop() {
    if (thread_.joinable()) {
        thread_.join();
    }
}

void RiskManagerThread::monitor() {
    while (running_) {
        if (risk_engine_->isBankrupt()) {
            std::cerr << "[RISK] Account Bankrupt. Triggering shutdown.\n";
            running_ = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

} // namespace XAlgo
