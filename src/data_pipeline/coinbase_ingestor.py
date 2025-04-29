# /src/data_pipeline/coinbase_ingestor.py

import asyncio
import websockets
import json
import logging
import time
from datetime import datetime

# Configure basic logger
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("coinbase_ingestor")

# Coinbase WebSocket Endpoint
COINBASE_WS_URL = "wss://ws-feed.exchange.coinbase.com"

# Trading pairs to monitor (Coinbase uses uppercase with hyphens)
PAIRS = ["BTC-USD", "ETH-USD", "ETH-BTC"]

# Build subscription payload
def build_subscription():
    return {
        "type": "subscribe",
        "channels": [
            {"name": "level2", "product_ids": PAIRS},
            {"name": "matches", "product_ids": PAIRS}
        ]
    }

async def handle_message(message: dict):
    type_ = message.get('type')

    if type_ == 'l2update':
        logger.info(f"[ORDERBOOK] {message['product_id']} | Changes: {message['changes']}")

    elif type_ == 'match':
        logger.info(f"[TRADE] {message['product_id']} | Price: {message['price']} | Size: {message['size']} | Side: {message['side']}")

async def connect_and_listen():
    while True:
        try:
            async with websockets.connect(COINBASE_WS_URL, ping_interval=20, ping_timeout=10) as websocket:
                logger.info("Connected to Coinbase WebSocket.")

                # Send subscription message
                subscribe_msg = build_subscription()
                await websocket.send(json.dumps(subscribe_msg))

                while True:
                    raw = await websocket.recv()
                    message = json.loads(raw)

                    if message.get('type') in {'l2update', 'match'}:
                        await handle_message(message)

        except (websockets.exceptions.ConnectionClosed, asyncio.TimeoutError) as e:
            logger.warning(f"Connection lost, retrying in 5 seconds: {e}")
            await asyncio.sleep(5)

if __name__ == "__main__":
    try:
        asyncio.run(connect_and_listen())
    except KeyboardInterrupt:
        logger.info("Shutdown requested, exiting...")
    except Exception as e:
        logger.error(f"Unhandled exception: {e}")
