#include "strategy.hpp"

#include <cmath>
#include <cstdint>
#include <string_view>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

struct SymbolState {
    double mids[64]{};
    double sum = 0.0;
    double sum_sq = 0.0;
    uint32_t count = 0;       
    uint32_t head = 0;        
    int32_t position = 0;     
};

class SpecStrategy : public csot::Strategy {
private:
    SymbolState states[64];
    string_view symbols[64];
    uint32_t symbol_count = 0;
    SymbolState& state_for(string_view sym){
        for (uint32_t i = 0; i < symbol_count; ++i){
            if (symbols[i] == sym){
                return states[i];
            }
        }
        symbols[symbol_count] = sym;
        return states[symbol_count++];
    }
public:
    std::vector<csot::Order> on_tick(const csot::Tick& t) {
        auto& st = state_for(t.symbol);
        const double mid = (t.bid_px + t.ask_px) * 0.5;
        const double old = st.mids[st.head];
        st.sum -= old;
        st.sum_sq -= old * old;
        st.mids[st.head] = mid;
        st.sum += mid;
        st.sum_sq += mid * mid;
        st.head = (st.head + 1) & 63;      
        if (st.count < 64) {
            ++st.count;
        }
        if (st.count < 64) {
            return {};
        }
        const double mean = st.sum / 64.0;
        const double variance = max(0.0,(st.sum_sq / 64.0)- mean * mean);
        const double stddev = std::sqrt(variance);
        if (stddev < 1e-9) {
            return {};
        }
        const double z = (mid - mean) / stddev;
        const double abs_z = std::abs(z);
        if (st.position == 0) {
            if (z >= 2.0) {
                return {csot::Order{csot::Order::Side::SELL, t.symbol, t.bid_px, 1}};
            }
            if (z <= -2.0) {
                return {csot::Order{csot::Order::Side::BUY, t.symbol, t.ask_px, 1}};
            }
            return {};
        }
        if (st.position > 0 && abs_z <= 0.5) {
            return {csot::Order{csot::Order::Side::SELL, t.symbol, t.bid_px,static_cast<uint32_t>(st.position)}};
        }
        if (st.position < 0 && abs_z <= 0.5) {
            return {csot::Order{csot::Order::Side::BUY, t.symbol, t.ask_px,static_cast<uint32_t>(-st.position)}};
        }
        return {};
    }
    void on_fill(const csot::Order& o,double,uint32_t fill_qty) override{
        auto& st = state_for(o.symbol);
        if (o.side == csot::Order::Side::BUY) {
            st.position += static_cast<int32_t>(fill_qty);
        }
        else{
            st.position -= static_cast<int32_t>(fill_qty);
        }
    }
};
extern "C" csot::Strategy* create_strategy() { return new SpecStrategy(); }
