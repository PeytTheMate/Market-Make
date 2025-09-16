#pragma once
#include "lib/base/types.hpp"

namespace mm {

class OrderBook {
 public:
  void on_quote(const BookSnapshot& b) { snap_ = b; }
  const BookSnapshot& snapshot() const { return snap_; }
 private:
  BookSnapshot snap_{};
};

} // namespace mm
