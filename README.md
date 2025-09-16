# Market Maker Lab (C++) – Skeleton

This is a production-style skeleton for a market making lab. It compiles without external deps
and gives you a clean place to drop in real components later (OMS, Prometheus, YAML, etc.).

## Quick start

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j
./mm_sim
```

## Layout

- `lib/` common types/utilities
- `md/`   market data contracts + CSV replayer
- `book/` limit order book (toy, for wiring)
- `strat/` strategy interface + Avellaneda–Stoikov baseline
- `risk/` pre-trade/running limits
- `acct/` PnL and inventory
- `sim/`  simple exchange simulator
- `apps/` runnables (sim & tools)
- `cfg/`  example configs
- `data/` tiny sample ticks CSV

See `DESIGN.md` for contracts & extension points.
