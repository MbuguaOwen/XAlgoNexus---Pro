# /src/strategy_core/signal_generator.py

import logging

# Configure logger
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("signal_generator")

class SignalGenerator:
    def __init__(self, spread_threshold=0.001, volatility_multiplier=2.0):
        self.spread_threshold = spread_threshold  # Base threshold for signal triggering
        self.volatility_multiplier = volatility_multiplier  # Adjust signal sensitivity to market volatility

    def generate_signal(self, features):
        if features is None:
            return None

        spread = features.get("spread")
        volatility = features.get("volatility")

        # Volatility-adjusted dynamic threshold
        dynamic_threshold = self.spread_threshold + (volatility * self.volatility_multiplier)

        # Decision logic
        if spread > dynamic_threshold:
            signal = "SELL ETH, BUY BTC"
        elif spread < -dynamic_threshold:
            signal = "BUY ETH, SELL BTC"
        else:
            signal = "HOLD"

        logger.info(f"[SIGNAL] Decision: {signal} | Spread: {spread:.6f} | Threshold: {dynamic_threshold:.6f}")
        
        return {
            "timestamp": features.get("timestamp"),
            "decision": signal,
            "spread": spread,
            "volatility": volatility,
            "threshold": dynamic_threshold
        }

if __name__ == "__main__":
    logger.info("Signal Generator Ready.")
