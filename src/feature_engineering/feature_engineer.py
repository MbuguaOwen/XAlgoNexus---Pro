# /src/feature_engineering/feature_engineer.py

import numpy as np
import pandas as pd
from collections import deque
import logging
import psycopg2
from psycopg2.extras import execute_values

# Logger setup
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("feature_engineer")


class FeatureEngineer:
    """
    Computes real-time engineered features from normalized trade events.
    Supports spread analysis and basic volatility/imbalance tracking.
    """

    def __init__(self, db_conn=None, maxlen=500):
        self.prices = {
            'btcusdt': deque(maxlen=maxlen),
            'ethusdt': deque(maxlen=maxlen),
            'ethbtc': deque(maxlen=maxlen)
        }
        self.spreads = deque(maxlen=maxlen)
        self.timestamps = deque(maxlen=maxlen)
        self.db_conn = db_conn

    def update(self, event):
        """
        Receives a normalized event and updates internal buffers.
        If sufficient data is present, calculates feature vector.
        """
        if event.event_type != 'trade':
            return None

        pair = event.pair
        if pair not in self.prices:
            return None

        self.prices[pair].append(float(event.price))
        self.timestamps.append(event.timestamp)

        if self.ready():
            fv = self.calculate_features()
            if fv and self.db_conn:
                self.insert_to_db(fv)
            return fv

        return None

    def ready(self):
        """
        Determines if enough data is buffered for feature calculation.
        """
        return all(len(q) > 10 for q in self.prices.values())

    def calculate_features(self):
        """
        Computes spread, volatility, and synthetic imbalance values.
        """
        try:
            btc_usdt = self.prices['btcusdt'][-1]
            eth_usdt = self.prices['ethusdt'][-1]
            eth_btc = self.prices['ethbtc'][-1]

            implied_ethbtc = eth_usdt / btc_usdt
            spread = eth_btc - implied_ethbtc
            self.spreads.append(spread)

            volatility = np.std(self.spreads) if len(self.spreads) > 5 else 0.0
            imbalance = np.random.uniform(-1, 1)  # 🔄 Replace with real imbalance if available

            feature_vector = {
                "timestamp": self.timestamps[-1],
                "spread": spread,
                "volatility": volatility,
                "imbalance": imbalance
            }

            logger.info(f"[FEATURE_VECTOR] {feature_vector}")
            return feature_vector

        except Exception as e:
            logger.error(f"[FEATURE_CALCULATION_ERROR] {e}")
            return None

    def insert_to_db(self, fv):
        """
        Inserts feature vector into the TimescaleDB feature_vectors table.
        """
        try:
            with self.db_conn.cursor() as cur:
                cur.execute("""
                    INSERT INTO feature_vectors (timestamp, spread, volatility, imbalance)
                    VALUES (%s, %s, %s, %s)
                """, (
                    fv['timestamp'],
                    float(fv['spread']),
                    float(fv['volatility']),
                    float(fv['imbalance'])
                ))
            self.db_conn.commit()

        except Exception as e:
            logger.error(f"[DB_INSERT_ERROR] {e}")
            self.db_conn.rollback()


if __name__ == "__main__":
    logger.info("[XALGO] Feature Engineer Ready.")
