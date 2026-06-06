#include "engine.hpp"

#include <cstdint>
#include <fstream>
#include <sstream>

using namespace std;

vector<csot::Tick> Engine::load_ticks(const string& path){
    vector<csot::Tick> ticks;
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)){
        stringstream ss(line);
        string ts;
        string sym;
        string bid;
        string ask;
        string bid_qty;
        string ask_qty;
        getline(ss,ts,',');
        getline(ss,sym,',');
        getline(ss,bid,',');
        getline(ss,ask,',');
        getline(ss,bid_qty,',');
        getline(ss,ask_qty,',');
        string_view symbol_view;
        auto it = interned.find(sym);
        if (it == interned.end()){
            symbol_storage.push_back(sym);
            symbol_view = symbol_storage.back();
            interned.emplace(symbol_storage.back(),symbol_view);
        }
        else {
            symbol_view = it->second;
        }
        csot::Tick tick;
        tick.timestamp_ns = stoull(ts);
        tick.symbol = symbol_view;
        tick.bid_px = stod(bid);
        tick.ask_px = stod(ask);
        tick.bid_qty = static_cast<uint32_t>(stoul(bid_qty));
        tick.ask_qty = static_cast<uint32_t>(stoul(ask_qty));
        ticks.push_back(tick);
    }
    return ticks;
}
