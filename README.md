# XAlgoNexus - High-Frequency Trading Framework

🚀 **XAlgoNexus** is an ultra-low-latency, modular High-Frequency Trading (HFT) simulation framework designed for real-time Forex triangular arbitrage strategies.

---

## 📦 Project Structure

```
src/
├── core/
│   ├── ingest/           # Historical tick ingestion
│   ├── preprocess/       # Tick preprocessing
│   ├── spread/           # Spread calculation (Triangular Arbitrage)
│   ├── signal/           # Signal generation (Buy/Sell)
│   ├── execution/        # Order execution simulation
│   ├── risk/             # Risk management & shutdown control
│   ├── logger/           # Trade logging to CSV
│   └── messaging/        # Lock-free message bus and events
├── data/                 # Historical tick data (CSV files)
├── third_party/          # moodycamel::ConcurrentQueue
└── main.cpp              # System orchestrator
```

... (shortened for display here, full included in file)