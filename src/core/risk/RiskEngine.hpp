// src/core/risk/RiskEngine.hpp

#pragma once

/**
 * @file RiskEngine.hpp
 * @brief Handles account capital, lot sizing, and bankruptcy detection.
 */

namespace XAlgo {

/**
 * @class RiskEngine
 * @brief Manages trading risk exposure and monitors available capital.
 */
class RiskEngine {
public:
    /**
     * @brief Constructor.
     * @param risk_percent Risk per trade expressed as a percentage of capital.
     * @param initial_capital Starting account balance.
     */
    RiskEngine(double risk_percent, double initial_capital);

    /**
     * @brief Get the current capital value.
     * @return Available capital.
     */
    [[nodiscard]] double getCapital() const;

    /**
     * @brief Update capital after a trade's profit or loss.
     * @param pnl Profit or loss from the last trade.
     */
    void updateCapital(double pnl);

    /**
     * @brief Calculate the lot size based on current capital and risk settings.
     * @return Lot size.
     */
    [[nodiscard]] double getLotSize() const;

    /**
     * @brief Determine if the account has gone bankrupt.
     * @return True if capital <= 0, else false.
     */
    [[nodiscard]] bool isBankrupt() const;

private:
    double risk_percent_; ///< Risk per trade (% of current capital).
    double capital_; ///< Current account balance.
};

} // namespace XAlgo
