-- /infra/timescaledb_schema.sql

-- Enable TimescaleDB extension
CREATE EXTENSION IF NOT EXISTS timescaledb;

-- Table for raw trade events
CREATE TABLE IF NOT EXISTS trade_events (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMPTZ NOT NULL,
    exchange TEXT NOT NULL,
    pair TEXT NOT NULL,
    price NUMERIC(18,8) NOT NULL,
    quantity NUMERIC(18,8) NOT NULL,
    side TEXT NOT NULL
);

SELECT create_hypertable('trade_events', 'timestamp', if_not_exists => TRUE);

-- Table for orderbook snapshots
CREATE TABLE IF NOT EXISTS orderbook_events (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMPTZ NOT NULL,
    exchange TEXT NOT NULL,
    pair TEXT NOT NULL,
    bids TEXT,
    asks TEXT
);

SELECT create_hypertable('orderbook_events', 'timestamp', if_not_exists => TRUE);

-- Table for engineered feature vectors
CREATE TABLE IF NOT EXISTS feature_vectors (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMPTZ NOT NULL,
    spread NUMERIC(18,8),
    volatility NUMERIC(18,8),
    imbalance NUMERIC(18,8)
);

SELECT create_hypertable('feature_vectors', 'timestamp', if_not_exists => TRUE);

-- Table for executed orders
CREATE TABLE IF NOT EXISTS execution_orders (
    id SERIAL PRIMARY KEY,
    order_id UUID NOT NULL,
    timestamp TIMESTAMPTZ NOT NULL,
    decision TEXT NOT NULL,
    requested_price NUMERIC(18,8),
    filled_price NUMERIC(18,8),
    slippage NUMERIC(10,8),
    trade_value_usd NUMERIC(18,8),
    status TEXT NOT NULL
);

SELECT create_hypertable('execution_orders', 'timestamp', if_not_exists => TRUE);

-- Indexes for faster querying
CREATE INDEX IF NOT EXISTS idx_trade_events_pair ON trade_events(pair);
CREATE INDEX IF NOT EXISTS idx_orderbook_events_pair ON orderbook_events(pair);
CREATE INDEX IF NOT EXISTS idx_feature_vectors_spread ON feature_vectors(spread);
CREATE INDEX IF NOT EXISTS idx_execution_orders_order_id ON execution_orders(order_id);
