#pragma once
#include "lib/base/types.hpp"

namespace mm {

struct OrderCmd {
  Side side{Side::Buy};
  Price px{0};
  Qty   qty{0};
  i64   ts_ns{0};
};

class IOrderGateway {
 public:
  virtual ~IOrderGateway() = default;
  virtual ExecReport route(const OrderCmd& cmd, const BookSnapshot& b) = 0;
};

} // namespace mm
