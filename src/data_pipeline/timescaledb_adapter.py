# /src/data_pipeline/timescaledb_adapter.py

import asyncpg
import asyncio
import logging

# Configure logger
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("timescaledb_adapter")

class TimescaleDBAdapter:
    def __init__(self, db_config):
        self.db_config = db_config
        self.pool = None

    async def init_pool(self):
        self.pool = await asyncpg.create_pool(**self.db_config)
        logger.info("[DB] Connection pool initialized.")

    async def insert_trade_event(self, event):
        query = """
        INSERT INTO trade_events (timestamp, exchange, pair, price, quantity, side)
        VALUES ($1, $2, $3, $4, $5, $6)
        """
        async with self.pool.acquire() as conn:
            await conn.execute(query,
                event.timestamp,
                event.exchange,
                event.pair,
                event.price,
                event.quantity,
                event.side)

    async def insert_orderbook_event(self, event):
        query = """
        INSERT INTO orderbook_events (timestamp, exchange, pair, bids, asks)
        VALUES ($1, $2, $3, $4, $5)
        """
        async with self.pool.acquire() as conn:
            await conn.execute(query,
                event.timestamp,
                event.exchange,
                event.pair,
                str(event.bids),
                str(event.asks))

    async def insert_feature_vector(self, feature):
        query = """
        INSERT INTO feature_vectors (timestamp, spread, volatility, imbalance)
        VALUES ($1, $2, $3, $4)
        """
        async with self.pool.acquire() as conn:
            await conn.execute(query,
                feature["timestamp"],
                feature["spread"],
                feature["volatility"],
                feature["imbalance"])

    async def insert_execution_order(self, order):
        query = """
        INSERT INTO execution_orders (order_id, timestamp, decision, requested_price, filled_price, slippage, trade_value_usd, status)
        VALUES ($1, $2, $3, $4, $5, $6, $7, $8)
        """
        async with self.pool.acquire() as conn:
            await conn.execute(query,
                order["order_id"],
                order["timestamp"],
                order["decision"],
                order["requested_price"],
                order["filled_price"],
                order["slippage"],
                order["trade_value_usd"],
                order["status"])

if __name__ == "__main__":
    logger.info("TimescaleDB Adapter Ready.")

    # Example usage
    # db_config = {
    #     'user': 'postgres',
    #     'password': 'yourpassword',
    #     'database': 'xalgo_trading',
    #     'host': 'localhost',
    #     'port': 5432
    # }
    # adapter = TimescaleDBAdapter(db_config)
    # asyncio.run(adapter.init_pool())
