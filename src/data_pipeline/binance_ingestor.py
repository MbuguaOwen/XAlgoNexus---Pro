# /src/data_pipeline/binance_ingestor.py

import asyncio
import websockets
import json
import logging
import time
from datetime import datetime

# Configure basic logger
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("binance_ingestor")

# Binance WebSocket Endpoints
BINANCE_WS_URL = "wss://stream.binance.com:9443/stream"

# Trading pairs to monitor
PAIRS = ["btcusdt", "ethusdt", "ethbtc"]

# Build subscription payload
def build_subscription():
    streams = [f"{pair}@depth5@100ms" for pair in PAIRS] + [f"{pair}@trade" for pair in PAIRS]
    return {
        "method": "SUBSCRIBE",
        "params": streams,
        "id": int(time.time())
    }

async def handle_message(message: dict):
    event_type = message.get('e')

    if event_type == 'depthUpdate':
        logger.info(f"[ORDERBOOK] {message['s']} | Bids: {message['b'][0]} | Asks: {message['a'][0]}")

    elif event_type == 'trade':
        logger.info(f"[TRADE] {message['s']} | Price: {message['p']} | Quantity: {message['q']}")

async def connect_and_listen():
    uri = f"{BINANCE_WS_URL}?streams=" + "/".join([f"{pair}@depth5@100ms/{pair}@trade" for pair in PAIRS])
    
    while True:
        try:
            async with websockets.connect(uri, ping_interval=20, ping_timeout=10) as websocket:
                logger.info("Connected to Binance WebSocket.")

                while True:
                    raw = await websocket.recv()
                    message = json.loads(raw)

                    if 'data' in message:
                        await handle_message(message['data'])

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
