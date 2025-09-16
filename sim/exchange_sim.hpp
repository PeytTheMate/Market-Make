#pragma once
#include "oms/sim_gateway.hpp"
#include "risk/risk_checks.hpp"
#include "acct/accounting.hpp"
#include "strat/strategy.hpp"

namespace mm {

// Wires: strategy -> risk -> gateway -> accounting
class ExchangeSim {
 public:
  ExchangeSim(RiskConfig rcfg) : rcfg_(rcfg) {}

  ExecReport try_execute(IStrategy& strat, const BookSnapshot& b, i64 ts_ns) {
    Quote q = strat.on_tick(b, ts_ns);
    // Attempt bid
    if (q.bid.has_value()) {
      if (pretrade_ok(rcfg_, rstate_, Side::Buy, b, *q.bid, q.bid_sz)) {
        OrderCmd cmd{Side::Buy, *q.bid, q.bid_sz, ts_ns};
        auto er = gw_.route(cmd, b);
        if (er.filled) { acct_.apply_fill(er); strat.on_fill(er); }
      }
    }
    // Attempt ask
    if (q.ask.has_value()) {
      if (pretrade_ok(rcfg_, rstate_, Side::Sell, b, *q.ask, q.ask_sz)) {
        OrderCmd cmd{Side::Sell, *q.ask, q.ask_sz, ts_ns};
        auto er = gw_.route(cmd, b);
        if (er.filled) { acct_.apply_fill(er); strat.on_fill(er); }
      }
    }

    // Update inventory state & mtm
    ledger_.inventory = acct_.ledger.inventory;
    rstate_.inventory = ledger_.inventory;
    acct_.mark_to_market(ledger_.inventory >= 0 ? ledger_.inventory, b); // keep function

    // Return last report (simplified)
    ExecReport last;
    last.ts_ns = ts_ns;
    return last;
  }

  struct Account {
    Ledger ledger{};
  } acct_;

  const Ledger& ledger() const { return acct_.ledger; }

 private:
  RiskConfig rcfg_;
  RiskState rstate_;
  SimGateway gw_;
  Ledger ledger_{};
};

} // namespace mm
