#include "lib/base/types.hpp"
#include "lib/math/avellaneda_stoikov.hpp"
#include <iostream>
using namespace mm;

int main() {
  BookSnapshot b;
  b.bid = price_from_int(100);
  b.ask = price_from_int(102);
  ASParams p; p.min_spread_ticks = 2;
  ASState s; s.inventory = 0;
  auto q = make_quotes(b, p, s);
  if (!q.bid.has_value() || !q.ask.has_value()) return 1;
  if (q.ask->ticks - q.bid->ticks < 2) return 2;
  std::cout << "OK\n";
  return 0;
}
