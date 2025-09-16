#pragma once
#include "lib/base/types.hpp"
#include <algorithm>

namespace mm {

struct RiskConfig {
  i64 max_inventory{1000};
  i64 max_order_qty{100};
  i64 price_band_ticks{100};
};

struct RiskState {
  i64 inventory{0};
};

inline bool within_price_band(const BookSnapshot& b, const Price& p, i64 band_ticks) {
  const i64 mid = mid_ticks(b);
  const i64 d = std::llabs(p.ticks - mid);
  return d <= band_ticks;
}

inline bool pretrade_ok(const RiskConfig& cfg, const RiskState& st, Side side, const BookSnapshot& b, const Price& p, const Qty& q) {
  if (q.lots <= 0 || q.lots > cfg.max_order_qty) return false;
  if (!within_price_band(b, p, cfg.price_band_ticks)) return false;
  // inventory band check (predictive)
  i64 inv_next = st.inventory + (side == Side::Buy ? q.lots : -q.lots);
  if (std::llabs(inv_next) > cfg.max_inventory) return false;
  return true;
}

} // namespace mm
