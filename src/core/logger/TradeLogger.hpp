#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <mutex>

namespace XAlgo {

class TradeLogger {
public:
    explicit TradeLogger(const std::string& filename);

    void logTrade(double spread, double pnl, bool win);
    void flush();

private:
    void writeHeaderIfNeeded(std::ofstream& file);

    std::string filename_;
    std::vector<std::tuple<double, double, bool>> buffer_;
    mutable std::mutex mtx_;
    bool header_written_;
};

} // namespace XAlgo
