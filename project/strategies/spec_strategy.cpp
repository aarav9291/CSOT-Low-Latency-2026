#include "strategy.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <string_view>
#include <vector>

using namespace std;

class SpecStrategy : public csot::Strategy {
private:
    struct SymbolState {
        double mids[64]{};
        uint32_t count = 0;
        uint32_t head = 0;
        int32_t position = 0;
    };

    array<SymbolState, 64> states_;
    array<string_view, 64> symbols_;
    uint32_t used_ = 0;

    SymbolState& state_for(string_view sym)
    {
        for (uint32_t i = 0; i < used_; ++i) {
            if (symbols_[i] == sym) {
                return states_[i];
            }
        }

        symbols_[used_] = sym;
        return states_[used_++];
    }

public:
    vector<csot::Order> on_tick(const csot::Tick& t) override
    {
        auto& st = state_for(t.symbol);

        const double mid =
            (t.bid_px + t.ask_px) * 0.5;

        st.mids[st.head] = mid;
        st.head = (st.head + 1) & 63;

        if (st.count < 64) {
            ++st.count;
        }

        if (st.count < 64) {
            return {};
        }

        double sum = 0.0;

        for (double x : st.mids) {
            sum += x;
        }

        const double mean = sum / 64.0;

        double sq = 0.0;

        for (double x : st.mids) {
            const double d = x - mean;
            sq += d * d;
        }

        const double variance = sq / 64.0;
        const double stddev = sqrt(variance);

        if (stddev < 1e-9) {
            return {};
        }

        const double z =
            (mid - mean) / stddev;

        const double abs_z =
            abs(z);

        if (st.position == 0) {

            if (z >= 2.0) {
                return {
                    csot::Order{
                        csot::Order::Side::SELL,
                        t.symbol,
                        t.bid_px,
                        1
                    }
                };
            }

            if (z <= -2.0) {
                return {
                    csot::Order{
                        csot::Order::Side::BUY,
                        t.symbol,
                        t.ask_px,
                        1
                    }
                };
            }

            return {};
        }

        if (st.position > 0 &&
            abs_z <= 0.5)
        {
            return {
                csot::Order{
                    csot::Order::Side::SELL,
                    t.symbol,
                    t.bid_px,
                    static_cast<uint32_t>(
                        st.position
                    )
                }
            };
        }

        if (st.position < 0 &&
            abs_z <= 0.5)
        {
            return {
                csot::Order{
                    csot::Order::Side::BUY,
                    t.symbol,
                    t.ask_px,
                    static_cast<uint32_t>(
                        -st.position
                    )
                }
            };
        }

        return {};
    }

    void on_fill(const csot::Order& o,
                 double,
                 uint32_t fill_qty) override
    {
        auto& st = state_for(o.symbol);

        if (o.side ==
            csot::Order::Side::BUY)
        {
            st.position +=
                static_cast<int32_t>(
                    fill_qty
                );
        }
        else {
            st.position -=
                static_cast<int32_t>(
                    fill_qty
                );
        }
    }
};

extern "C" csot::Strategy* create_strategy()
{
    return new SpecStrategy();
}
