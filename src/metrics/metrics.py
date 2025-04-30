from prometheus_client import Gauge

spread_gauge = Gauge('xalgo_latest_spread', 'Latest spread value')
volatility_gauge = Gauge('xalgo_latest_volatility', 'Latest volatility value')
imbalance_gauge = Gauge('xalgo_latest_imbalance', 'Latest imbalance value')
pnl_gauge = Gauge('xalgo_daily_pnl', 'Daily cumulative PnL')
