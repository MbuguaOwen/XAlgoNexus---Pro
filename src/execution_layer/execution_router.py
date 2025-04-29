# /src/execution_layer/execution_router.py

import logging
import random
import uuid
from datetime import datetime

# Configure logger
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("execution_router")

class ExecutionRouter:
    def __init__(self, slippage_basis_points=5):
        self.slippage_basis_points = slippage_basis_points  # Default slippage = 0.05%
        self.orders_executed = []

    def simulate_order_execution(self, signal, base_price, quantity_usd):
        """
        Simulate an order execution based on signal, price, and quantity.
        """
        if signal is None or signal['decision'] == "HOLD":
            logger.info("[EXECUTION] No action for HOLD signal.")
            return None

        # Apply random slippage
        slippage_percent = random.uniform(0, self.slippage_basis_points) / 10000
        fill_price = base_price * (1 + slippage_percent) if "BUY" in signal['decision'] else base_price * (1 - slippage_percent)

        order = {
            "order_id": str(uuid.uuid4()),
            "timestamp": datetime.utcnow().isoformat(),
            "decision": signal['decision'],
            "requested_price": base_price,
            "filled_price": fill_price,
            "slippage": slippage_percent,
            "trade_value_usd": quantity_usd,
            "status": "FILLED"
        }

        self.orders_executed.append(order)

        logger.info(f"[EXECUTION] Executed Order: {order}")
        return order

    def get_last_order(self):
        if self.orders_executed:
            return self.orders_executed[-1]
        return None

if __name__ == "__main__":
    logger.info("Execution Router Ready.")
