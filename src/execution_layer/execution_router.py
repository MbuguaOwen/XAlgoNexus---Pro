# /src/execution_layer/execution_router.py

import logging
import random
import uuid
from datetime import datetime

# Logger setup
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("execution_router")


class ExecutionRouter:
    """
    Simulates or routes trade executions based on generated signals.
    Handles slippage, logging, and internal tracking of orders.
    """

    def __init__(self, slippage_basis_points: float = 5.0):
        """
        Args:
            slippage_basis_points (float): Slippage in bps (e.g., 5 = 0.05%)
        """
        self.slippage_basis_points = slippage_basis_points
        self.orders_executed = []

    def simulate_order_execution(self, signal: dict, base_price: float, quantity_usd: float) -> dict | None:
        """
        Simulates execution with slippage.

        Args:
            signal (dict): Trade signal with 'decision'
            base_price (float): Mid-price or quoted spread
            quantity_usd (float): Trade notional in USD

        Returns:
            dict: Order execution payload (simulated)
        """
        if not signal or signal.get("decision", "HOLD") == "HOLD":
            logger.debug("[EXECUTION] Skipping HOLD signal.")
            return None

        # Simulated slippage (bps range → decimal %)
        slippage_pct = random.uniform(0, self.slippage_basis_points) / 10000

        direction = signal["decision"].upper()
        fill_price = (
            base_price * (1 + slippage_pct) if "BUY" in direction
            else base_price * (1 - slippage_pct)
        )

        order = {
            "order_id": str(uuid.uuid4()),
            "timestamp": datetime.utcnow(),
            "decision": direction,
            "requested_price": round(base_price, 8),
            "filled_price": round(fill_price, 8),
            "slippage": round(slippage_pct, 8),
            "trade_value_usd": round(quantity_usd, 2),
            "status": "FILLED"
        }

        self.orders_executed.append(order)
        logger.info(f"[EXECUTION] Order Executed: {order}")
        return order

    def get_last_order(self) -> dict | None:
        """
        Returns:
            Most recent simulated order, if any
        """
        return self.orders_executed[-1] if self.orders_executed else None


if __name__ == "__main__":
    logger.info("[XALGO] Execution Router Ready.")
