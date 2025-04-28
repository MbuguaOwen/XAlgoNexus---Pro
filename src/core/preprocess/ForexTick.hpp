// src/core/preprocess/ForexTick.hpp

#pragma once

/**
 * @file ForexTick.hpp
 * @brief Struct representing a single Forex tick event.
 */

#include <string>
#include <chrono>

namespace XAlgo {

/**
 * @struct ForexTick
 * @brief Describes a single Forex market tick with timestamp, bid, ask, and mid-price calculation.
 */
struct ForexTick {
    std::chrono::system_clock::time_point timestamp; ///< Timestamp of the tick.
    std::string symbol; ///< Symbol (e.g., "GBP/USD").
    double bid; ///< Best bid price.
    double ask; ///< Best ask price.

    /**
     * @brief Compute the mid-price (average of bid and ask).
     * @return Mid-price value.
     */
    double mid() const {
        return (bid + ask) * 0.5;
    }
};

} // namespace XAlgo
