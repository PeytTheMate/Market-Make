#pragma once
#include "lib/base/types.hpp"
#include <string>

namespace mm {

enum class MdType : std::uint8_t { Quote = 0, Trade = 1 };

struct MdEvent {
  MdType type{MdType::Quote};
  BookSnapshot book{};   // for Quote
  Price trade_px{0};     // for Trade
  Qty   trade_qty{0};    // for Trade
};

} // namespace mm
