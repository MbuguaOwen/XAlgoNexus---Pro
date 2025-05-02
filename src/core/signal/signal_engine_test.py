
from core.signal.signal_engine import SignalEngine
from core.filters.ml_filter import MLFilter

engine = SignalEngine(ml_filter=MLFilter())

example_fv = {
    "spread_zscore": 2.6,
    "volatility": 1.9e-6,
    "imbalance": 0.87
}

engine.process(example_fv)

# Simulate Execution Hook
for sig in engine.get_signals():
    print("[Executor] Processing:", sig)
