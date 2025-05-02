
from datetime import datetime
from prometheus_client import Counter

# Prometheus metrics
execution_counter = Counter('xalgo_trades_executed_total', 'Total trades executed', ['mode', 'type'])

class ExecutionEngine:
    def __init__(self, mode="paper", logger=None):
        self.mode = mode  # "live" or "paper"
        self.logger = logger or print
        self.trade_log = []

    def execute(self, signal):
        trade_details = {
            "timestamp": datetime.utcnow().isoformat(),
            "action": signal.signal_type,
            "reason": signal.reason,
            "metadata": signal.metadata
        }

        execution_counter.labels(mode=self.mode, type=signal.signal_type).inc()

        if self.mode == "live":
            self.logger(f"[LIVE] Executing: {trade_details}")
        else:
            self.logger(f"[PAPER] Executed: {trade_details}")

        self.trade_log.append(trade_details)

    def get_trade_log(self):
        return self.trade_log
