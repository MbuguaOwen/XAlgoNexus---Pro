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
            side=msg['m'] and 'sell' or 'buy'
        )

    @staticmethod
    def normalize_binance_orderbook(msg: dict) -> NormalizedEvent:
        best_bid = float(msg['b'][0][0]) if msg['b'] else None
        best_ask = float(msg['a'][0][0]) if msg['a'] else None
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

    @staticmethod
    def normalize_coinbase_trade(msg: dict) -> NormalizedEvent:
        return NormalizedEvent(
            timestamp=datetime.strptime(msg['time'], "%Y-%m-%dT%H:%M:%S.%fZ"),
            exchange="coinbase",
            event_type="trade",
            pair=msg['product_id'].replace('-', '').lower(),
            price=float(msg['price']),
            quantity=float(msg['size']),
            side=msg['side']
        )

    @staticmethod
    def normalize_coinbase_orderbook(msg: dict) -> NormalizedEvent:
        changes = msg['changes']
        # Coinbase sends [side, price, size] updates
        side, price, quantity = changes[0]
        return NormalizedEvent(
            timestamp=datetime.strptime(msg['time'], "%Y-%m-%dT%H:%M:%S.%fZ"),
            exchange="coinbase",
            event_type="orderbook",
            pair=msg['product_id'].replace('-', '').lower(),
            price=float(price),
            quantity=float(quantity),
            side=side
        )

if __name__ == "__main__":
    logger.info("Data Normalizer Module Ready.")
