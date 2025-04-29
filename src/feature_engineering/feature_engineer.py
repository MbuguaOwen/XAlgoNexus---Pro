# /src/feature_engineering/feature_engineer.py

import numpy as np
import pandas as pd
from collections import deque
import logging

# Configure logger
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("feature_engineer")

class FeatureEngineer:
    def __init__(self, maxlen=500):
        # Rolling windows to store recent events
        self.prices = {
            'btcusdt': deque(maxlen=maxlen),
            'ethusdt': deque(maxlen=maxlen),
            'ethbtc': deque(maxlen=maxlen)
        }
        self.spreads = deque(maxlen=maxlen)
        self.timestamps = deque(maxlen=maxlen)

    def update(self, event):
        if event.event_type != 'trade':
            return  # Only use trades for basic price features

        pair = event.pair
        if pair in self.prices:
            self.prices[pair].append(float(event.price))
            self.timestamps.append(event.timestamp)

            if self.ready():
                self.calculate_features()

    def ready(self):
        return all(len(q) > 10 for q in self.prices.values())

    def calculate_features(self):
        try:
            # Latest prices
            btc_usdt = self.prices['btcusdt'][-1]
            eth_usdt = self.prices['ethusdt'][-1]
            eth_btc = self.prices['ethbtc'][-1]

            # Calculate spread: expected ETH/BTC price vs actual
            implied_ethbtc = eth_usdt / btc_usdt
            spread = eth_btc - implied_ethbtc
            self.spreads.append(spread)

            # Volatility (standard deviation of spreads)
            vol = np.std(list(self.spreads)) if len(self.spreads) > 5 else 0.0

            # Orderbook imbalance (dummy placeholder)
            # Real implementation would use real-time L2 data
            imbalance = np.random.uniform(-1, 1)  # Simulate imbalance [-1,1]

            feature_vector = {
                "timestamp": self.timestamps[-1],
                "spread": spread,
                "volatility": vol,
                "imbalance": imbalance
            }

            logger.info(f"[FEATURES] {feature_vector}")
            return feature_vector

        except Exception as e:
            logger.error(f"Error in feature calculation: {e}")
            return None

if __name__ == "__main__":
    logger.info("Feature Engineer Ready.")
