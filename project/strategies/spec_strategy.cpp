#include "strategy.hpp"

#include <vector>

using namespace std;

class NullStrategy : public csot::Strategy {
public:
    vector<csot::Order> on_tick(const csot::Tick&) override{
        return {};
    }
};
extern "C" csot::Strategy* create_strategy() { return new NullStrategy(); }
