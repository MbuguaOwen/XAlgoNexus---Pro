# /src/data_pipeline/binance_ingestor.py

import asyncio
import websockets
import json
import logging
import time
from data_pipeline.data_normalizer import DataNormalizer

# Configure logger
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("binance_ingestor")

PAIRS = ["btcusdt", "ethusdt", "ethbtc"]
WS_URL = "wss://stream.binance.com:9443/ws"

def build_subscription():
    """
    Constructs the Binance WebSocket subscription message.
    """
    streams = [f"{pair}@depth5@100ms" for pair in PAIRS] + [f"{pair}@trade" for pair in PAIRS]
    return {
        "method": "SUBSCRIBE",
        "params": streams,
        "id": int(time.time())
    }

async def handle_message(message: dict, process_event_func):
    """
    Handles a single parsed WebSocket event and passes it through the normalization + processing pipeline.
    """
    try:
        event_type = message.get("e")

        if event_type == "trade":
            event = DataNormalizer.normalize_binance_trade(message)
            await process_event_func(event)
            logger.info(f"[TRADE] {event.pair} | Price: {event.price} | Qty: {event.quantity}")

        elif event_type == "depthUpdate":
            event = DataNormalizer.normalize_binance_orderbook(message)
            await process_event_func(event)
            logger.info(f"[ORDERBOOK] {event.pair} | Bids: {event.bids[0]} | Asks: {event.asks[0]}")

        else:
            logger.debug(f"[BINANCE] Ignored event type: {event_type}")

    except Exception as e:
        logger.exception(f"[BINANCE] Failed to handle message: {e}")

async def connect_and_listen(process_event_func):
    """
    Connects to Binance WebSocket and subscribes to desired market data streams.
    For each message, it routes it to the handler pipeline.
    """
    logger.info("[BINANCE] Connecting to WebSocket...")

    while True:
        try:
            async with websockets.connect(WS_URL, ping_interval=20, ping_timeout=10) as websocket:
                logger.info("[BINANCE] Connected. Sending subscription request...")
                await websocket.send(json.dumps(build_subscription()))
                logger.info("[BINANCE] Subscribed to streams.")

                while True:
                    raw = await websocket.recv()
                    message = json.loads(raw)

                    # Handle both raw and combined stream formats
                    if "e" in message:
                        await handle_message(message, process_event_func)
                    elif "data" in message:
                        await handle_message(message["data"], process_event_func)
                    else:
                        logger.debug(f"[BINANCE] Unrecognized message format: {message}")

        except (websockets.exceptions.ConnectionClosed, asyncio.TimeoutError) as e:
            logger.warning(f"[BINANCE] Disconnected. Reconnecting in 5s... Reason: {e}")
            await asyncio.sleep(5)

        except Exception as e:
            logger.exception(f"[BINANCE] Unexpected failure: {e}")
            await asyncio.sleep(5)

if __name__ == "__main__":
    logger.warning("Run this module from live_controller.py by passing in process_event().")
