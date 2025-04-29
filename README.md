# XAlgo BTC/ETH/USDT Trading System - Initial README

# Project Overview
The XAlgo BTC/ETH/USDT Trading System is a high-frequency, adaptive trading platform focusing exclusively on Bitcoin, Ethereum, and Tether (USDT) markets. Built with modularity, low-latency, and real-time machine learning integration in mind, it aims to achieve consistent profits by leveraging spread inefficiencies and adaptive strategy selection.

# System Architecture
```
Market APIs --> Data Collector --> Normalizer --> Feature Engineering -->
Strategy Core + ML Inference --> Smart Order Router --> Exchange Execution -->
Monitoring & Risk Management
```

# Technology Stack
- Language: Python 3.11+
- Backend: FastAPI (async API integration)
- Database: TimescaleDB (tick and orderbook storage)
- ML Framework: PyTorch + Scikit-learn
- Queue: Kafka (event streaming)
- Monitoring: Prometheus + Grafana
- Logging: Elasticsearch + Kibana
- Deployment: Docker + Kubernetes

# Directory Structure
```
/src
    /data_pipeline
    /strategy_core
    /ml_engine
    /execution_layer
    /risk_manager
    /monitoring
    /utils
/tests
/docs
/config
Dockerfile
requirements.txt
README.md
```

# Quick Start (Development Environment)

## 1. Clone Repository
```bash
git clone https://github.com/yourorg/xalgo-btc-eth-usdt.git
cd xalgo-btc-eth-usdt
```

## 2. Setup Python Environment
```bash
python3.11 -m venv venv
source venv/bin/activate
pip install --upgrade pip
pip install -r requirements.txt
```

## 3. Setup TimescaleDB (Local)
```bash
docker-compose up -d timescaledb
```
(Configure DB access in `/config/database.yaml`)

## 4. Run Local Development Server (FastAPI)
```bash
uvicorn src.main:app --reload --host 0.0.0.0 --port 8000
```

API Documentation will be available at: `http://localhost:8000/docs`

# Primary Components
- **Data Pipeline**: Ingests tick data from Binance & Coinbase.
- **Feature Engineering**: Extracts real-time features like spread, volatility.
- **Strategy Core**: Executes stat arb and ML-driven decisions.
- **ML Inference Engine**: Applies trained RL models for meta-strategy switching.
- **Execution Layer**: Sends and manages orders.
- **Risk Manager**: Enforces stop-losses, position sizing, risk caps.
- **Monitoring**: Real-time dashboards, alerts, system health tracking.

# Development Flow
1. **Ingest tick/order book data**
2. **Build feature extraction (spread calculations, liquidity measures)**
3. **Deploy basic arbitrage and spread-based strategies**
4. **Integrate machine learning agent for dynamic strategy switching**
5. **Connect to exchanges for live trading (Binance/Coinbase APIs)**
6. **Activate real-time monitoring and alert systems**
7. **Optimize latency, system reliability, and scaling**

# Security
- Secrets are stored using HashiCorp Vault.
- All API keys are encrypted at rest.
- OAuth 2.0 authentication on internal endpoints.

# License
Copyright (c) 2025 XAlgoNexus

---

# 🚀 Let's Build!
- Day 1: Data ingestion setup
- Day 2: Tick data normalization
- Day 3: Feature engineering (spread, volatility metrics)
- Day 4: Strategy prototyping (simple stat arb)
- Day 5: ML agent integration (meta-strategy switching)

---

# Contacts
- Core Team: core@xalgonexus.com
- Emergency Support: ops@xalgonexus.com
