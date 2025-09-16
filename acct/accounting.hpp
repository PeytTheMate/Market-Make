#pragma once
#include "lib/base/types.hpp"

namespace mm {

struct Ledger {
  i64 inventory{0};      // lots
  i64 cash_ticks{0};     // cash in tick*lots units (negative when buying)
  i64 unrealized_ticks{0};
  i64 realized_ticks() const { return cash_ticks; } // simple version
};

inline void apply_fill(Ledger& l, const ExecReport& r) {
  if (!r.filled) return;
  if (r.side == Side::Buy) {
    l.inventory += r.qty.lots;
    l.cash_ticks -= r.px.ticks * r.qty.lots;
  } else {
    l.inventory -= r.qty.lots;
    l.cash_ticks += r.px.ticks * r.qty.lots;
  }
}

inline void mark_to_market(Ledger& l, const BookSnapshot& b) {
  const i64 mark = mid_ticks(b);
  l.unrealized_ticks = l.inventory * mark;
}

} // namespace mm
