// src/core/logger/TradeLogger.cpp

#include "core/logger/TradeLogger.hpp"
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <iostream>

namespace XAlgo {

TradeLogger::TradeLogger(const std::string& filename)
{
    file_.open(filename, std::ios::out | std::ios::trunc);
    if (!file_.is_open()) {
        std::cerr << "[ERROR] Unable to open trade log file: " << filename << "\n";
        return;
    }

    // Write CSV header
    file_ << "timestamp,symbol,side,price,size,pnl,capital\n";
}

TradeLogger::~TradeLogger() {
    if (file_.is_open()) {
        file_.close();
    }
}

void TradeLogger::logTrade(const std::string& symbol,
                           const std::string& side,
                           double price,
                           double size,
                           double pnl,
                           double capital) {
    if (!file_.is_open()) return;

    file_ << getCurrentTimestamp() << ","
          << symbol << ","
          << side << ","
          << price << ","
          << size << ","
          << pnl << ","
          << capital << "\n";

    file_.flush(); // Optional: flush to disk immediately after each trade
}

std::string TradeLogger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm utc_tm;
#if defined(_WIN32) || defined(_WIN64)
    gmtime_s(&utc_tm, &now_time);
#else
    gmtime_r(&now_time, &utc_tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

} // namespace XAlgo
