#include "engine.hpp"

#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
    Engine engine;
    auto ticks = engine.load_ticks(argv[1]);
    cout << ticks.size() << '\n';
    cout << "First tick:\n";
    cout << ticks.front().timestamp_ns << " "
         << ticks.front().symbol << " "
         << ticks.front().bid_px << " "
         << ticks.front().ask_px << " "
         << ticks.front().bid_qty << " "
         << ticks.front().ask_qty << '\n';
    cout << "Last tick:\n";
    cout << ticks.back().timestamp_ns << " "
         << ticks.back().symbol << " "
         << ticks.back().bid_px << " "
         << ticks.back().ask_px << " "
         << ticks.back().bid_qty << " "
         << ticks.back().ask_qty << '\n';
    cout << "ticks[0].symbol = "
         << ticks[0].symbol
         << '\n';
    return 0;
}
