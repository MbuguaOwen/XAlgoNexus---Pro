# /src/main/live_controller.py

import asyncio
import logging

from data_pipeline.binance_ingestor import connect_and_listen as binance_feed
from data_pipeline.coinbase_ingestor import connect_and_listen as coinbase_feed
from data_pipeline.data_normalizer import DataNormalizer
from feature_engineering.feature_engineer import FeatureEngineer
from strategy_core.signal_generator import SignalGenerator
from risk_manager.risk_manager import RiskManager
from execution_layer.execution_router import ExecutionRouter
from data_pipeline.timescaledb_adapter import TimescaleDBAdapter

# Configure logger
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("live_controller")

# Initialize system components
normalizer = DataNormalizer()
feature_engineer = FeatureEngineer()
signal_generator = SignalGenerator()
risk_manager = RiskManager()
execution_router = ExecutionRouter()

db_config = {
    'user': 'postgres',
    'password': 'yourpassword',
    'database': 'xalgo_trading',
    'host': 'localhost',
    'port': 5432
}

storage_adapter = TimescaleDBAdapter(db_config)

async def process_event(event):
    try:
        # Save raw market event
        if event.event_type == 'trade':
            await storage_adapter.insert_trade_event(event)
        elif event.event_type == 'orderbook':
            await storage_adapter.insert_orderbook_event(event)

        # Feed into feature engineer
        feature = feature_engineer.update(event)
        if feature:
            await storage_adapter.insert_feature_vector(feature)

            # Generate trading signal
            signal = signal_generator.generate_signal(feature)

            # Risk check and execution
            if signal and signal['decision'] != 'HOLD':
                base_price = feature['spread']  # Placeholder: use real price feed for execution
                quantity_usd = 1000.0  # Example fixed amount per trade
                slippage_estimate = 0.0005  # Placeholder slippage

                if risk_manager.check_trade_permission(signal, quantity_usd, slippage_estimate):
                    order = execution_router.simulate_order_execution(signal, base_price, quantity_usd)
                    if order:
                        await storage_adapter.insert_execution_order(order)
                        risk_manager.update_pnl((order['filled_price'] - order['requested_price']) * quantity_usd)

    except Exception as e:
        logger.error(f"[LIVE_CONTROLLER] Error processing event: {e}")

async def main():
    logger.info("Starting Live Controller...")
    await storage_adapter.init_pool()

    # Run Binance and Coinbase listeners concurrently
    binance_task = asyncio.create_task(binance_feed())
    coinbase_task = asyncio.create_task(coinbase_feed())

    await asyncio.gather(binance_task, coinbase_task)

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("Shutdown requested by user.")
    except Exception as e:
        logger.error(f"Fatal error in Live Controller: {e}")
