#pragma once
#include "strat/strategy.hpp"
#include "lib/math/avellaneda_stoikov.hpp"

namespace mm {

class AvellanedaStoikov final : public IStrategy {
 public:
  explicit AvellanedaStoikov(ASParams p) : params_(p) {}

  Quote on_tick(const BookSnapshot& b, i64 /*ts_ns*/) override {
    ASState s;
    s.inventory = inv_;
    return make_quotes(b, params_, s);
  }

  void on_fill(const ExecReport& r) override {
    if (!r.filled) return;
    inv_ += (r.side == Side::Buy ? r.qty.lots : -r.qty.lots);
  }

 private:
  ASParams params_;
  i64 inv_{0};
};

} // namespace mm
