# /src/main/live_controller.py

import asyncio
import logging
from fastapi import FastAPI
from fastapi.responses import Response
from prometheus_client import generate_latest, CONTENT_TYPE_LATEST, start_http_server

from metrics.metrics import spread_gauge, volatility_gauge, imbalance_gauge, pnl_gauge

from data_pipeline.data_normalizer import DataNormalizer
from feature_engineering.feature_engineer import FeatureEngineer
from strategy_core.signal_generator import SignalGenerator
from risk_manager.risk_manager import RiskManager
from execution_layer.execution_router import ExecutionRouter
from data_pipeline.timescaledb_adapter import TimescaleDBAdapter

# ✅ Delayed import to prevent circular import
from data_pipeline import binance_ingestor

# Logging setup
logging.basicConfig(
    level=logging.INFO,
    format="[%(asctime)s] %(levelname)s - %(name)s - %(message)s"
)
logger = logging.getLogger("live_controller")

# FastAPI instance
app = FastAPI()

# Component initialization
normalizer = DataNormalizer()
feature_engineer = FeatureEngineer()
signal_generator = SignalGenerator()
risk_manager = RiskManager()
execution_router = ExecutionRouter()

# DB connection settings for Docker-based TimescaleDB
db_config = {
    'user': 'postgres',
    'password': '11230428018',
    'database': 'xalgo_trading_db',
    'host': 'timescaledb',
    'port': 5432
}

# Global DB adapter
storage_adapter = TimescaleDBAdapter(db_config)

# ✅ Event Handler
async def process_event(event):
    try:
        if event.event_type == 'trade':
            await storage_adapter.insert_trade_event(event)
        elif event.event_type == 'orderbook':
            await storage_adapter.insert_orderbook_event(event)

        # Feature generation
        feature = feature_engineer.update(event)
        if feature:
            await storage_adapter.insert_feature_vector(feature)

            # Update Prometheus metrics
            spread_gauge.set(feature["spread"])
            volatility_gauge.set(feature["volatility"])
            imbalance_gauge.set(feature["imbalance"])

            # Generate and simulate signals
            signal = signal_generator.generate_signal(feature)
            if signal and signal["decision"] != "HOLD":
                base_price = feature["spread"]  # ⚠️ Consider replacing with midprice
                quantity_usd = 1000.0
                slippage = 0.0005

                if risk_manager.check_trade_permission(signal, quantity_usd, slippage):
                    order = execution_router.simulate_order_execution(signal, base_price, quantity_usd)
                    if order:
                        await storage_adapter.insert_execution_order(order)
                        pnl_delta = (order['filled_price'] - order['requested_price']) * quantity_usd
                        risk_manager.update_pnl(pnl_delta)
                        pnl_gauge.set(risk_manager.daily_pnl)

    except Exception as e:
        logger.error(f"[LIVE_CONTROLLER] Event processing failed: {e}")

# ✅ Pipeline Bootstrap
async def start_pipeline():
    logger.info("[XALGO] Bootstrapping components...")
    await storage_adapter.init_pool()
    await binance_ingestor.connect_and_listen(process_event)

# ✅ Prometheus HTTP endpoint for FastAPI
@app.get("/metrics")
def metrics():
    return Response(generate_latest(), media_type=CONTENT_TYPE_LATEST)

# ✅ Entrypoint
if __name__ == "__main__":
    import uvicorn

    async def run_all():
        await asyncio.gather(
            start_pipeline(),
            uvicorn.Server(
                uvicorn.Config(app, host="0.0.0.0", port=9100, log_level="info")
            ).serve()
        )

    try:
        start_http_server(9090)  # Optional: standalone Prometheus HTTP
        asyncio.run(run_all())
    except KeyboardInterrupt:
        logger.info("[XALGO] Graceful shutdown.")
    except Exception as e:
        logger.exception(f"[XALGO] Fatal error during startup: {e}")
