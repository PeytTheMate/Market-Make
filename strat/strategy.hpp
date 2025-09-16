#pragma once
#include "lib/base/types.hpp"

namespace mm {

class IStrategy {
 public:
  virtual ~IStrategy() = default;
  virtual Quote on_tick(const BookSnapshot& b, i64 ts_ns) = 0;
  virtual void on_fill(const ExecReport& ) {}
};

} // namespace mm
