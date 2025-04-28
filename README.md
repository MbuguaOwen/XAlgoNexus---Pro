# XAlgoNexus

**High-Frequency Trading Framework for Short-Term Triangular Arbitrage and Statistical Strategies**

---

## 🚀 Overview

**XAlgoNexus** is a modular, ultra-low-latency high-frequency trading (HFT) system designed for:

- **Triangular arbitrage** in Forex and Crypto markets
- **Statistical arbitrage** using real-time volatility and spread dynamics
- **Machine learning–enhanced signal filtering**
- **Backtesting, Paper Trading, and Live Execution Ready**

Every module is optimized for:
- Sub-microsecond tick processing
- Lock-free concurrency
- Robust risk management
- High-precision trade logging
- Modular extensibility

Built with **C++17**, clean RAII patterns, Doxygen-ready documentation, and designed for **production deployments**.

---

## 🛠️ System Architecture

```text
Data Ingestor → Tick Preprocessor → Spread Engine → Signal Generator → Risk Engine → Execution Engine → Trade Logger


src/
├── core/
│   ├── common/            # Shared utilities (EventQueue, ReplayMode)
│   ├── execution/         # Trade execution logic
│   ├── filters/           # Volatility estimation
│   ├── ingest/            # Historical and live market data ingestion
│   ├── logger/            # Trade logging subsystem
│   ├── messaging/         # Lock-free message bus and market event types
│   ├── preprocess/        # Tick normalization and preprocessing
│   ├── risk/              # Risk management core
│   ├── signal/            # Signal generation based on spread and ML filters
│   └── spread/            # Triangular spread calculation engine
└── main.cpp               # Entry point and orchestration
