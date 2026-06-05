#include "engine.hpp"
#include <memory>

using namespace std;

extern "C" csot::Strategy* create_strategy();

int main(int argc, char* argv[]){
    auto ticks = load_ticks(argv[1]);
    unique_ptr<csot::Strategy> strategy(create_strategy());
    strategy->on_init();
    replay_ticks(ticks, *strategy);
    return 0;
}
