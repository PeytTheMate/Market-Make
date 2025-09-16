#pragma once
#include "md/market_data.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace mm {

class CsvReplayer {
 public:
  explicit CsvReplayer(std::string path) : path_(std::move(path)) {}

  bool read_all(std::vector<MdEvent>& out) {
    std::ifstream in(path_);
    if (!in.is_open()) return false;
    std::string line;
    while (std::getline(in, line)) {
      if (line.empty() || line[0] == '#') continue;
      std::stringstream ss(line);
      std::string tok;
      // ts_ns,bid,ask,bid_sz,ask_sz,last,last_sz
      long long ts = 0; long long bid=0, ask=0, bsz=0, asz=0, last=0, lsz=0;
      char comma;
      if (!(ss >> ts >> comma >> bid >> comma >> ask >> comma >> bsz >> comma >> asz >> comma >> last >> comma >> lsz)) {
        continue;
      }
      MdEvent e;
      e.type = MdType::Quote;
      e.book.ts_ns = ts;
      e.book.bid = Price{bid};
      e.book.ask = Price{ask};
      e.book.bid_sz = Qty{bsz};
      e.book.ask_sz = Qty{asz};
      out.push_back(e);
      // also emit a trade event for variety (optional)
      MdEvent t;
      t.type = MdType::Trade;
      t.book.ts_ns = ts;
      t.trade_px = Price{last};
      t.trade_qty = Qty{lsz};
      out.push_back(t);
    }
    return true;
  }

 private:
  std::string path_;
};

} // namespace mm
