#include "core/logger/TradeLogger.hpp"
#include <fstream>
#include <iostream>

namespace XAlgo {

TradeLogger::TradeLogger(const std::string& filename)
    : filename_(filename), header_written_(false)
{}

void TradeLogger::logTrade(double spread, double pnl, bool win) {
    std::lock_guard<std::mutex> lock(mtx_);
    buffer_.emplace_back(spread, pnl, win);
}

void TradeLogger::flush() {
    std::lock_guard<std::mutex> lock(mtx_);

    std::ofstream file(filename_, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Cannot open trade log file: " << filename_ << "\n";
        return;
    }

    if (!header_written_) {
        writeHeaderIfNeeded(file);
        header_written_ = true;
    }

    for (const auto& trade : buffer_) {
        file << std::get<0>(trade) << ","   // spread
             << std::get<1>(trade) << ","   // pnl
             << (std::get<2>(trade) ? 1 : 0) // win (bool to 0/1)
             << "\n";
    }

    buffer_.clear();
}

void TradeLogger::writeHeaderIfNeeded(std::ofstream& file) {
    file << "spread,pnl,win\n";
}

} // namespace XAlgo
