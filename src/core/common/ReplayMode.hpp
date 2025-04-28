// src/core/common/ReplayMode.hpp

#pragma once

/**
 * @file ReplayMode.hpp
 * @brief Enumerates modes for controlling historical data replay speed during backtesting.
 */

namespace XAlgo {

/**
 * @enum ReplayMode
 * @brief Modes for historical data replay.
 */
enum class ReplayMode {
    REALTIME,    ///< Replay ticks with realistic real-time delays.
    FASTFORWARD, ///< Replay ticks as fast as possible with minimal delay.
    SLOWMOTION   ///< Replay ticks slower than real-time for detailed analysis.
};

} // namespace XAlgo
