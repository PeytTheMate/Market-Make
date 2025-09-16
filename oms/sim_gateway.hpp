#pragma once
#include "oms/gateway.hpp"
#include "lib/base/types.hpp"

namespace mm {

// Simplified: immediate-or-cancel against current book.
class SimGateway : public IOrderGateway {
 public:
  ExecReport route(const OrderCmd& cmd, const BookSnapshot& b) override {
    ExecReport r;
    r.side = cmd.side;
    r.ts_ns = cmd.ts_ns;
    // Fill if marketable vs opposite side; else no fill (reject silently as IOC)
    if (cmd.side == Side::Buy) {
      if (cmd.px.ticks >= b.ask.ticks && b.ask_sz.lots > 0) {
        r.filled = true; r.px = b.ask; r.qty = cmd.qty;
      }
    } else {
      if (cmd.px.ticks <= b.bid.ticks && b.bid_sz.lots > 0) {
        r.filled = true; r.px = b.bid; r.qty = cmd.qty;
      }
    }
    return r;
  }
};

} // namespace mm
