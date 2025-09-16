# Design Outline

This skeleton mirrors a real-firm architecture with clear module boundaries and deterministic
pipelines. All money is represented in integer ticks/lots.

## Contracts (headers)

- `lib/base/types.hpp` – strong types (`Price`, `Qty`, `Side`, ids), helpers.
- `md/market_data.hpp` – `MdEvent`, `QuoteTick`, `TradeTick`.
- `book/order_book.hpp` – minimal book snapshot + update hooks.
- `strat/strategy.hpp` – `IStrategy::on_tick()` returning desired `Quote`.
- `risk/risk_checks.hpp` – pure functions for limits; stateful inventory caps.
- `acct/accounting.hpp` – realized/unrealized PnL & inventory.
- `sim/exchange_sim.hpp` – fills quotes against the book (toy model).

`apps/mm_sim.cpp` wires a simple E2E: CSV ticks -> book -> strategy -> risk -> sim -> acct.

## Determinism

- Single-threaded loop in this skeleton; swap with actors/SPSC later.
- All randomness (if any) is seeded; sim currently deterministic (no RNG).

## Next steps

- Replace CSV with ITCH/OUCH or venue API.
- Real L2 order book & queue position.
- Real OMS (routes, rejections, acks) and Prometheus/OTel.
- YAML/TOML configs via your favorite lib.
