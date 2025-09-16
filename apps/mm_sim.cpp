#include "obs/log.hpp"
#include "md/csv_replayer.hpp"
#include "book/order_book.hpp"
#include "strat/avellaneda_stoikov.hpp"
#include "sim/exchange_sim.hpp"
#include "acct/accounting.hpp"
#include <vector>
#include <iostream>
#include <iomanip>

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

  OrderBook book;
  ASParams p;
  p.gamma = 0.001;
  p.sigma = 1.0;
  p.horizon_s = 1.0;
  p.min_spread_ticks = 2;
  p.base_size = qty_from_int(10);
  AvellanedaStoikov strat(p);

  RiskConfig rcfg;
  rcfg.max_inventory = 1000;
  rcfg.max_order_qty = 50;
  rcfg.price_band_ticks = 100;

  ExchangeSim sim(rcfg);

  long steps = 0;
  for (const auto& e : evs) {
    if (e.type == MdType::Quote) {
      book.on_quote(e.book);
      sim.try_execute(strat, book.snapshot(), e.book.ts_ns);
      Ledger led = sim.ledger();
      apply_fill(led, ExecReport{}); // no-op to link symbol
      mark_to_market(led, book.snapshot());
      if ((++steps % 2) == 0) {
        std::cout << "t=" << e.book.ts_ns
                  << " inv=" << led.inventory
                  << " pnl(real)ticks=" << led.realized_ticks()
                  << " mtm=" << led.unrealized_ticks
                  << " mid=" << mid_ticks(book.snapshot())
                  << "\n";
      }
    }
  }

  log_info("Done.");
  return 0;
}
