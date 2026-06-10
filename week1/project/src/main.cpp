#include "engine.hpp"
#include "strategy.hpp"

#include <memory>

using namespace std;

extern "C" csot::Strategy* create_strategy();

int main(int argc, char* argv[]){
    Engine engine;
    auto ticks = engine.load_ticks(argv[1]);
    unique_ptr<csot::Strategy> strategy(create_strategy());
    strategy->on_init();
    engine.tick_loop(ticks,*strategy);
    return 0;
}
