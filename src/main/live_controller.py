# /src/main/live_controller.py

import asyncio
import logging
from fastapi import FastAPI
from fastapi.responses import Response
from prometheus_client import generate_latest, CONTENT_TYPE_LATEST
from metrics.metrics import spread_gauge, volatility_gauge, imbalance_gauge, pnl_gauge

from data_pipeline.binance_ingestor import connect_and_listen
from data_pipeline.data_normalizer import DataNormalizer
from feature_engineering.feature_engineer import FeatureEngineer
from strategy_core.signal_generator import SignalGenerator
from risk_manager.risk_manager import RiskManager
from execution_layer.execution_router import ExecutionRouter
from data_pipeline.timescaledb_adapter import TimescaleDBAdapter

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format="[%(asctime)s] %(levelname)s - %(name)s - %(message)s"
)
logger = logging.getLogger("live_controller")

# FastAPI for /metrics
app = FastAPI()

# Core pipeline components
normalizer = DataNormalizer()
feature_engineer = FeatureEngineer()
signal_generator = SignalGenerator()
risk_manager = RiskManager()
execution_router = ExecutionRouter()

# TimescaleDB config
db_config = {
    'user': 'postgres',
    'password': '11230428018',
    'database': 'xalgo_trading',
    'host': 'localhost',
    'port': 5432
}

storage_adapter = TimescaleDBAdapter(db_config)

# Core event processor
async def process_event(event):
    try:
        if event.event_type == 'trade':
            await storage_adapter.insert_trade_event(event)
        elif event.event_type == 'orderbook':
            await storage_adapter.insert_orderbook_event(event)

        feature = feature_engineer.update(event)
        if feature:
            await storage_adapter.insert_feature_vector(feature)

            spread_gauge.set(feature["spread"])
            volatility_gauge.set(feature["volatility"])
            imbalance_gauge.set(feature["imbalance"])

            signal = signal_generator.generate_signal(feature)

            if signal and signal['decision'] != 'HOLD':
                base_price = feature['spread']  # Placeholder — replace with fair price input
                quantity_usd = 1000.0
                slippage_estimate = 0.0005

                if risk_manager.check_trade_permission(signal, quantity_usd, slippage_estimate):
                    order = execution_router.simulate_order_execution(signal, base_price, quantity_usd)
                    if order:
                        await storage_adapter.insert_execution_order(order)
                        pnl_delta = (order['filled_price'] - order['requested_price']) * quantity_usd
                        risk_manager.update_pnl(pnl_delta)
                        pnl_gauge.set(risk_manager.daily_pnl)

    except Exception as e:
        logger.error(f"[LIVE_CONTROLLER] Event processing failed: {e}")

# Start pipeline and WebSocket ingest
async def main():
    logger.info("[XALGO] Starting Live Controller...")
    await storage_adapter.init_pool()
    binance_task = asyncio.create_task(connect_and_listen(process_event))
    await asyncio.gather(binance_task)

# Expose Prometheus metrics
@app.get("/metrics")
def metrics():
    return Response(generate_latest(), media_type=CONTENT_TYPE_LATEST)

# Entrypoint
if __name__ == "__main__":
    import uvicorn

    async def startup():
        await main()

    try:
        loop = asyncio.get_event_loop()
        loop.create_task(startup())
        uvicorn.run("main.live_controller:app", host="0.0.0.0", port=9100, reload=False)
    except KeyboardInterrupt:
        logger.info("[XALGO] Shutdown by user.")
    except Exception as e:
        logger.error(f"[XALGO] Fatal error in controller: {e}")
