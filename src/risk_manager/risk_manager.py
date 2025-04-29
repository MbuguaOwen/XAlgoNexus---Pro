# /src/risk_manager/risk_manager.py

import logging
from datetime import datetime

# Configure logger
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("risk_manager")

class RiskManager:
    def __init__(self, max_position_size=5000.0, max_daily_loss=0.02, slippage_tolerance=0.0015):
        self.max_position_size = max_position_size  # Maximum USD value per trade
        self.max_daily_loss = max_daily_loss        # Max daily drawdown as fraction of capital
        self.slippage_tolerance = slippage_tolerance  # Tolerated slippage before rejecting execution
        self.current_exposure = 0.0                  # Current open exposure
        self.daily_pnl = 0.0                         # Cumulative daily P&L
        self.start_of_day = datetime.utcnow().date()

    def reset_daily_limits(self):
        today = datetime.utcnow().date()
        if today != self.start_of_day:
            logger.info("[RISK] Resetting daily limits.")
            self.daily_pnl = 0.0
            self.start_of_day = today

    def check_trade_permission(self, signal, estimated_trade_value, slippage_estimate):
        self.reset_daily_limits()

        # Check daily loss limit
        if self.daily_pnl < (-self.max_daily_loss):
            logger.warning("[RISK] Daily loss limit breached. No further trading allowed today.")
            return False

        # Check position sizing
        if estimated_trade_value > self.max_position_size:
            logger.warning(f"[RISK] Trade size {estimated_trade_value} exceeds maximum allowed {self.max_position_size}.")
            return False

        # Check slippage tolerance
        if slippage_estimate > self.slippage_tolerance:
            logger.warning(f"[RISK] Slippage {slippage_estimate} exceeds tolerance {self.slippage_tolerance}.")
            return False

        logger.info(f"[RISK] Trade approved: {signal['decision']} | Est Value: {estimated_trade_value:.2f} | Slippage: {slippage_estimate:.4f}")
        return True

    def update_pnl(self, realized_pnl):
        self.daily_pnl += realized_pnl
        logger.info(f"[RISK] Updated Daily PnL: {self.daily_pnl:.4f}")

if __name__ == "__main__":
    logger.info("Risk Manager Ready.")
