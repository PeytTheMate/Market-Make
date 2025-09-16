#pragma once
#include "lib/base/types.hpp"
#include <algorithm>
#include <cmath>

namespace mm {

struct ASParams {
  double gamma{0.001};     // risk aversion
  double sigma{1.0};       // volatility (ticks / sqrt(period))
  double horizon_s{1.0};   // horizon seconds
  i64    min_spread_ticks{2};
  Qty    base_size{qty_from_int(10)};
};

struct ASState {
  i64 inventory{0};
};

inline i64 optimal_spread_ticks(const ASParams& p) {
  // s* ~ gamma * sigma^2 * T  (coarse discrete version)
  double s = p.gamma * p.sigma * p.sigma * p.horizon_s;
  i64 s_ticks = static_cast<i64>(std::llround(s));
  return std::max(s_ticks, p.min_spread_ticks);
}

inline i64 reservation_price_ticks(i64 mid, const ASParams& p, const ASState& s) {
  // r = mid - q * gamma * sigma^2 * T
  double adj = static_cast<double>(s.inventory) * p.gamma * p.sigma * p.sigma * p.horizon_s;
  i64 r = static_cast<i64>(std::llround(static_cast<double>(mid) - adj));
  return r;
}

inline Quote make_quotes(const BookSnapshot& b, const ASParams& p, const ASState& s) {
  const i64 mid = mid_ticks(b);
  const i64 s_ticks = optimal_spread_ticks(p);
  const i64 r = reservation_price_ticks(mid, p, s);
  Quote q;
  q.bid = Price{r - s_ticks / 2};
  q.ask = Price{r + (s_ticks - s_ticks / 2)}; // ensure total s_ticks
  q.bid_sz = p.base_size;
  q.ask_sz = p.base_size;
  return q;
}

} // namespace mm
