#include "obs/log.hpp"
#include "md/csv_replayer.hpp"
#include <vector>
#include <iostream>
using namespace mm;

int main(int argc, char** argv) {
  std::string path = "data/sample_ticks.csv";
  if (argc > 1) path = argv[1];
  CsvReplayer rp(path);
  std::vector<MdEvent> evs;
  if (!rp.read_all(evs)) {
    log_err("Failed to read CSV: " + path);
    return 1;
  }
  for (const auto& e : evs) {
    if (e.type == MdType::Quote) {
      std::cout << "QUOTE t=" << e.book.ts_ns
                << " bid=" << e.book.bid.ticks
                << " ask=" << e.book.ask.ticks << "\n";
    } else {
      std::cout << "TRADE t=" << e.book.ts_ns
                << " px=" << e.trade_px.ticks
                << " qty=" << e.trade_qty.lots << "\n";
    }
  }
  return 0;
}
