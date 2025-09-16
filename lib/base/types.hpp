#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <limits>
#include <compare>

namespace mm {

using i64 = std::int64_t;
using u64 = std::uint64_t;

struct Price {
  i64 ticks{0};
  constexpr auto operator<=>(const Price&) const = default;
};
struct Qty {
  i64 lots{0};
  constexpr auto operator<=>(const Qty&) const = default;
};

enum class Side : std::uint8_t { Buy = 0, Sell = 1 };

struct Quote {
  // Optional to allow one-sided quoting
  std::optional<Price> bid;
  std::optional<Price> ask;
  Qty bid_sz{0};
  Qty ask_sz{0};
};

struct BookSnapshot {
  Price bid{0};
  Price ask{0};
  Qty bid_sz{0};
  Qty ask_sz{0};
  i64 ts_ns{0};
};

struct ExecReport {
  bool filled{false};
  Side side{Side::Buy};
  Price px{0};
  Qty   qty{0};
  i64   ts_ns{0};
};

inline Price price_from_int(i64 t) { return Price{t}; }
inline Qty   qty_from_int(i64 q)   { return Qty{q}; }

inline i64 mid_ticks(const BookSnapshot& b) {
  return (b.bid.ticks + b.ask.ticks) / 2;
}

inline std::string side_to_str(Side s) { return s == Side::Buy ? "BUY" : "SELL"; }

} // namespace mm
