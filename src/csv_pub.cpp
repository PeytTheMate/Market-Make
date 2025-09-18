#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <vector>
#include <iostream>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

extern "C" {
#  if __has_include(<nats/nats.h>)
#    include <nats/nats.h>
#  elif __has_include(<nats.h>)
#    include <nats.h>
#  else
#    error "Cannot find NATS headers. Ensure CMake added nats include dirs."
#  endif
}
#include "md_tick.hpp"

// fast-cpp-csv-parser (header-only)
#define CSV_IO_NO_THREAD
#include "csv.h"

struct Args {
    std::string csv_path;
    std::string nats_url = "nats://127.0.0.1:4222";
    std::string subject  = "md.tick";
    std::string source   = "csv_replay_v1";
    int          throttle_ms = 0; // sleep per row for replay pacing
};

static void die_if(natsStatus s, const char* msg) {
    if (s != NATS_OK) {
        fmt::print(stderr, "[NATS] {}: {}\n", msg, natsStatus_GetText(s));
        std::exit(2);
    }
}

static Args parse_args(int argc, char** argv) {
    Args a;
    for (int i = 1; i < argc; ++i) {
        std::string v = argv[i];
        auto next = [&](const char* flag) -> std::string {
            if (i + 1 >= argc) {
                fmt::print(stderr, "Missing value for {}\n", flag);
                std::exit(1);
            }
            return argv[++i];
        };

        if (v == "--csv")        a.csv_path = next("--csv");
        else if (v == "--nats")  a.nats_url = next("--nats");
        else if (v == "--subj")  a.subject  = next("--subj");
        else if (v == "--source")a.source   = next("--source");
        else if (v == "--sleep_ms") a.throttle_ms = std::stoi(next("--sleep_ms"));
        else if (v == "-h" || v == "--help") {
            fmt::print("Usage:\n"
                       "  csv_pub --csv ticks.csv [--nats nats://127.0.0.1:4222] "
                       "[--subj md.tick] [--source csv_replay_v1] [--sleep_ms 1]\n");
            std::exit(0);
        } else {
            fmt::print(stderr, "Unknown arg: {}\n", v);
            std::exit(1);
        }
    }
    if (a.csv_path.empty()) {
        fmt::print(stderr, "Required: --csv <path>\n");
        std::exit(1);
    }
    return a;
}

int main(int argc, char** argv) {
    Args args = parse_args(argc, argv);

    // --- Connect to NATS ---
    natsConnection* nc = nullptr;
    natsOptions*    opts = nullptr;
    die_if(natsOptions_Create(&opts), "natsOptions_Create");
    die_if(natsConnection_Connect(&nc, opts), "natsConnection_Connect");
    natsOptions_Destroy(opts);
    fmt::print("[csv_pub] Connected to NATS at {}\n", args.nats_url);
    // Note: the default Connect ignores the URL in opts if not set; set it:
    // We’ll reconnect with a URL to be explicit:
    natsConnection_Destroy(nc); nc = nullptr;
    die_if(natsOptions_Create(&opts), "natsOptions_Create(2)");
    die_if(natsOptions_SetURL(opts, args.nats_url.c_str()), "natsOptions_SetURL");
    die_if(natsConnection_Connect(&nc, opts), "natsConnection_Connect(2)");
    natsOptions_Destroy(opts);

    // --- CSV Reader ---
    // Expected headers (strict): see md_tick.hpp comments.
    // If your CSV is missing some fields, add placeholders for Day-1, or extend parsing below.
    io::CSVReader<10, io::trim_chars<' ','\t'>, io::no_quote_escape<','>>
        in(args.csv_path);

    in.read_header(io::ignore_no_column,
                   "ts_utc","symbol","bid","ask","last",
                   "bid_size","ask_size","last_size","venue","seq");

    std::string ts_utc, symbol, venue;
    double bid=0, ask=0, last=0;
    long long bid_size=0, ask_size=0, last_size=0;
    unsigned long long seq=0;

    uint64_t row_idx = 0;
    uint64_t sent = 0;

    while (in.read_row(ts_utc, symbol, bid, ask, last,
                       bid_size, ask_size, last_size, venue, seq)) {

        md_tick tick;
        tick.ts_utc = ts_utc;
        tick.symbol = symbol;
        tick.bid = bid;
        tick.ask = ask;
        tick.last = last;
        tick.bid_size = bid_size;
        tick.ask_size = ask_size;
        tick.last_size = last_size;
        tick.venue = venue;
        tick.seq = seq == 0 ? row_idx : seq; // fall back to row index if seq missing
        tick.source = args.source;

        nlohmann::json j = tick;
        const auto payload = j.dump(); // compact JSON

        natsStatus s = natsConnection_PublishString(nc, args.subject.c_str(), payload.c_str());
        if (s != NATS_OK) {
            fmt::print(stderr, "[csv_pub] publish failed at row {}: {}\n",
                       row_idx, natsStatus_GetText(s));
            // Fail fast on Day 1; later we can add retry/backoff.
            natsConnection_Flush(nc);
            natsConnection_Destroy(nc);
            return 3;
        }

        ++sent;
        ++row_idx;

        if (args.throttle_ms > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(args.throttle_ms));
    }

    die_if(natsConnection_Flush(nc), "natsConnection_Flush");
    natsConnection_Destroy(nc);

    fmt::print("[csv_pub] Done. Published {} ticks to '{}'.\n", sent, args.subject);
    return 0;
}
