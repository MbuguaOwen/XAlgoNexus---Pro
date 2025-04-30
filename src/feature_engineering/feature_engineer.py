import numpy as np
import pandas as pd
from collections import deque
import logging
import psycopg2
from psycopg2.extras import execute_values

# Configure logger
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("feature_engineer")

class FeatureEngineer:
    def __init__(self, db_conn=None, maxlen=500):
        self.prices = {
            'btcusdt': deque(maxlen=maxlen),
            'ethusdt': deque(maxlen=maxlen),
            'ethbtc': deque(maxlen=maxlen)
        }
        self.spreads = deque(maxlen=maxlen)
        self.timestamps = deque(maxlen=maxlen)
        self.db_conn = db_conn  # psycopg2 connection object

    def update(self, event):
        if event.event_type != 'trade':
            return

        pair = event.pair
        if pair in self.prices:
            self.prices[pair].append(float(event.price))
            self.timestamps.append(event.timestamp)

            if self.ready():
                features = self.calculate_features()
                if features and self.db_conn:
                    self.insert_to_db(features)

    def ready(self):
        return all(len(q) > 10 for q in self.prices.values())

    def calculate_features(self):
        try:
            btc_usdt = self.prices['btcusdt'][-1]
            eth_usdt = self.prices['ethusdt'][-1]
            eth_btc = self.prices['ethbtc'][-1]

            implied_ethbtc = eth_usdt / btc_usdt
            spread = eth_btc - implied_ethbtc
            self.spreads.append(spread)
            vol = np.std(list(self.spreads)) if len(self.spreads) > 5 else 0.0
            imbalance = np.random.uniform(-1, 1)

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

    def insert_to_db(self, fv):
        try:
            with self.db_conn.cursor() as cur:
                cur.execute("""
                    INSERT INTO feature_vectors (timestamp, spread, volatility, imbalance)
                    VALUES (%s, %s, %s, %s)
                """, (fv['timestamp'], float(fv['spread']), float(fv['volatility']), float(fv['imbalance'])))
            self.db_conn.commit()
        except Exception as e:
            logger.error(f"[DB INSERT ERROR] {e}")
            self.db_conn.rollback()

if __name__ == "__main__":
    logger.info("Feature Engineer Ready.")
