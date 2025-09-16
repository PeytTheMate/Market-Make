#pragma once
#include <cstdint>

namespace mm {
struct Clock {
  static std::int64_t now_ns_monotonic(); // implemented in .cpp if needed
};
}
