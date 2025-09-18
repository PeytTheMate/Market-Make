#pragma once
#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

// Initial md_tick schema. Keep explicit to avoid silent coercions.
struct md_tick {
    // Required
    std::string ts_utc;   // ISO 8601, e.g. "2025-09-18T15:04:05.123456Z"
    std::string symbol;   // ticker symbols

    // Optional-but-usually-present market microstructure fields
    double bid = 0.0;
    double ask = 0.0;
    double last = 0.0;
    int64_t bid_size = 0;
    int64_t ask_size = 0;
    int64_t last_size = 0;

    // Provenance / sequencing
    std::string venue;    // e.g. "XNAS", "CME", "SIM"
    uint64_t seq = 0;     // per-symbol sequence if present; else producer row index
    std::string source;   // e.g. "csv_replay_v1", "live_gateway"

    // Versioning for forward-compat. Increment on breaking changes.
    int schema_version = 1;
};

// JSON (de)serialization
inline void to_json(nlohmann::json& j, const md_tick& m) {
    j = nlohmann::json{
        {"schema_version", m.schema_version},
        {"ts_utc",         m.ts_utc},
        {"symbol",         m.symbol},
        {"bid",            m.bid},
        {"ask",            m.ask},
        {"last",           m.last},
        {"bid_size",       m.bid_size},
        {"ask_size",       m.ask_size},
        {"last_size",      m.last_size},
        {"venue",          m.venue},
        {"seq",            m.seq},
        {"source",         m.source}
    };
}

inline void from_json(const nlohmann::json& j, md_tick& m) {
    j.at("schema_version").get_to(m.schema_version);
    j.at("ts_utc").get_to(m.ts_utc);
    j.at("symbol").get_to(m.symbol);
    j.at("bid").get_to(m.bid);
    j.at("ask").get_to(m.ask);
    j.at("last").get_to(m.last);
    j.at("bid_size").get_to(m.bid_size);
    j.at("ask_size").get_to(m.ask_size);
    j.at("last_size").get_to(m.last_size);
    j.at("venue").get_to(m.venue);
    j.at("seq").get_to(m.seq);
    j.at("source").get_to(m.source);
}
