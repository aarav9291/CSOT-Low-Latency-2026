#include "engine.hpp"
#include "histogram.hpp"
#include <chrono>
#include <cstdint>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace std;
using namespace chrono;

static deque<string> symbol_storage;
static unordered_map<string, string_view> interned;

vector<csot::Tick> load_ticks(const string& path){
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
        auto it = interned.find(sym_s);
        if (it == interned.end()) {
            symbol_storage.push_back(sym_s);
            symbol_view = symbol_storage.back();
            interned.emplace(
                symbol_storage.back(),
                symbol_view
            );
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

void replay_ticks(const vector<csot::Tick>& ticks,csot::Strategy& strategy){
    csot::LatencyHistogram hist;
    for (const auto& tick : ticks) {
        auto t1 = steady_clock::now();
        auto orders = strategy.on_tick(tick);
        auto t2 = steady_clock::now();
        auto ns = duration_cast<nanoseconds>(t2 - t1).count();
        hist.record(static_cast<uint64_t>(ns));
        for (const auto& order : orders) {
            strategy.on_fill(order,order.price,order.qty);
        }
    }
    cout << ticks.size() << " ticks processed\n";
    hist.print(cout);
}
