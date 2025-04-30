# /src/data_pipeline/data_normalizer.py

import logging
from datetime import datetime

# Configure logger
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("data_normalizer")

class NormalizedEvent:
    def __init__(self, timestamp, exchange, event_type, pair, price, quantity, side=None, bids=None, asks=None):
        self.timestamp = timestamp
        self.exchange = exchange
        self.event_type = event_type  # 'trade' or 'orderbook'
        self.pair = pair
        self.price = price
        self.quantity = quantity
        self.side = side
        self.bids = bids
        self.asks = asks

    def to_dict(self):
        return {
            "timestamp": self.timestamp,
            "exchange": self.exchange,
            "event_type": self.event_type,
            "pair": self.pair,
            "price": self.price,
            "quantity": self.quantity,
            "side": self.side,
            "bids": self.bids,
            "asks": self.asks
        }

class DataNormalizer:

    @staticmethod
    def normalize_binance_trade(msg: dict) -> NormalizedEvent:
        return NormalizedEvent(
            timestamp=datetime.utcfromtimestamp(msg['T'] / 1000),
            exchange="binance",
            event_type="trade",
            pair=msg['s'].lower(),
            price=float(msg['p']),
            quantity=float(msg['q']),
            side='sell' if msg['m'] else 'buy'
        )

    @staticmethod
    def normalize_binance_orderbook(msg: dict) -> NormalizedEvent:
        return NormalizedEvent(
            timestamp=datetime.utcfromtimestamp(msg['E'] / 1000),
            exchange="binance",
            event_type="orderbook",
            pair=msg['s'].lower(),
            price=None,
            quantity=None,
            bids=msg['b'],
            asks=msg['a']
        )

if __name__ == "__main__":
    logger.info("Binance-only Data Normalizer Ready.")
