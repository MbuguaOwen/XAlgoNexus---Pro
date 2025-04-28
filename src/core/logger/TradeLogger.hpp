// src/core/logger/TradeLogger.hpp

#pragma once

/**
 * @file TradeLogger.hpp
 * @brief Responsible for persisting trade executions into structured CSV files.
 */

#include <string>
#include <fstream>

namespace XAlgo {

/**
 * @class TradeLogger
 * @brief Logs executed trades with full metadata into a CSV file.
 */
class TradeLogger {
public:
    /**
     * @brief Constructor.
     * @param filename Output CSV file name.
     */
    explicit TradeLogger(const std::string& filename);

    /**
     * @brief Destructor (ensures file is properly closed).
     */
    ~TradeLogger();

    /**
     * @brief Log a trade entry into the CSV file.
     * @param symbol Traded symbol (e.g., "EUR/USD").
     * @param side "BUY" or "SELL".
     * @param price Execution price.
     * @param size Trade lot size.
     * @param pnl Profit or loss from the trade.
     * @param capital Remaining account capital after the trade.
     */
    void logTrade(const std::string& symbol,
                  const std::string& side,
                  double price,
                  double size,
                  double pnl,
                  double capital);

private:
    std::ofstream file_; ///< Output file stream for CSV writing.

    /**
     * @brief Get current UTC timestamp as ISO 8601 formatted string.
     * @return Formatted timestamp string.
     */
    std::string getCurrentTimestamp() const;
};

} // namespace XAlgo
