import sys
import os
import asyncio
import logging
from fastapi import FastAPI
from fastapi.responses import Response
from prometheus_client import generate_latest, CONTENT_TYPE_LATEST, start_http_server

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from metrics.metrics import spread_gauge, volatility_gauge, imbalance_gauge, pnl_gauge
from data_pipeline.data_normalizer import DataNormalizer
from feature_engineering.feature_engineer import FeatureEngineer
from strategy_core.signal_generator import SignalGenerator
from risk_manager.risk_manager import RiskManager
from execution_layer.execution_router import ExecutionRouter
from execution_layer.pnl_tracker import PnLTracker
from data_pipeline.timescaledb_adapter import TimescaleDBAdapter
from data_pipeline import binance_ingestor  # Delayed import

# Logging setup
logging.basicConfig(
    level=logging.INFO,
    format="[%(asctime)s] %(levelname)s - %(name)s - %(message)s"
)
logger = logging.getLogger("live_controller")

# FastAPI app
app = FastAPI()

# Initialize components
normalizer = DataNormalizer()
feature_engineer = FeatureEngineer()
risk_manager = RiskManager()
execution_router = ExecutionRouter()
pnl_tracker = PnLTracker()

# Handle missing model safely
try:
    signal_generator = SignalGenerator()
except Exception as e:
    logger.warning(f"[MLFilter] Model initialization failed: {e}")
    
    class DummySignalGenerator:
        def generate_signal(self, feature):
            logger.warning("[DummySignal] Returning HOLD due to missing model.")
            return {"decision": "HOLD"}

    signal_generator = DummySignalGenerator()

db_config = {
    'user': 'postgres',
    'password': '11230428018',
    'database': 'xalgo_trading_db',
    'host': 'timescaledb',
    'port': 5432
}
storage_adapter = TimescaleDBAdapter(db_config)

# Main event loop
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
            if signal and signal["decision"] != "HOLD":
                base_price = feature["spread"]
                quantity_usd = 1000.0
                slippage = 0.0005

                if risk_manager.check_trade_permission(signal, quantity_usd, slippage):
                    order = execution_router.simulate_order_execution(signal, base_price, quantity_usd)
                    if order:
                        await storage_adapter.insert_execution_order(order)
                        pnl_tracker.update_position(
                            symbol=order['pair'],
                            fill_price=order['filled_price'],
                            qty=order['quantity'],
                            side=order['side']
                        )
                        pnl_tracker.mark_to_market({order['pair']: order['filled_price']})
                        risk_manager.update_pnl(pnl_tracker.get_total_pnl())
                        pnl_gauge.set(risk_manager.daily_pnl)
    except Exception as e:
        logger.error(f"[LIVE_CONTROLLER] Event processing failed: {e}")

# Startup
async def start_pipeline():
    logger.info("[XALGO] Bootstrapping components...")
    await storage_adapter.init_pool()
    await binance_ingestor.connect_and_listen(process_event)

@app.get("/metrics")
def metrics():
    return Response(generate_latest(), media_type=CONTENT_TYPE_LATEST)

@app.get("/pnl")
def get_pnl():
    return pnl_tracker.summary()

# Entry point
if __name__ == "__main__":
    start_http_server(9100)
    asyncio.run(start_pipeline())
    uvicorn.run(app, host="0.0.0.0", port=9100)
