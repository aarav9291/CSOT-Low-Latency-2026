#include "engine.hpp"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

vector<csot::Tick> Engine::load_ticks(const string& path) {
    vector<csot::Tick> ticks;
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        stringstream ss(line);
        string ts_s;
        string sym_s;
        string bid_s;
        string ask_s;
        string bid_qty_s;
        string ask_qty_s;
        getline(ss, ts_s, ',');
        getline(ss, sym_s, ',');
        getline(ss, bid_s, ',');
        getline(ss, ask_s, ',');
        getline(ss, bid_qty_s, ',');
        getline(ss, ask_qty_s, ',');
        string_view symbol_view;
        auto it = interned_.find(sym_s);
        if (it == interned_.end()) {
            symbol_storage_.push_back(sym_s);
            symbol_view = symbol_storage_.back();
            interned_.emplace(symbol_storage_.back(),symbol_view);
        }
        else {
            symbol_view = it->second;
        }
        csot::Tick tick;
        tick.timestamp_ns = stoull(ts_s);
        tick.symbol = symbol_view;
        tick.bid_px = stod(bid_s);
        tick.ask_px = stod(ask_s);
        tick.bid_qty = static_cast<uint32_t>(stoul(bid_qty_s));
        tick.ask_qty = static_cast<uint32_t>(stoul(ask_qty_s));
        ticks.push_back(tick);
    }
    return ticks;
}
