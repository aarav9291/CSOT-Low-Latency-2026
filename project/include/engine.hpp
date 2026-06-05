#pragma once
#include "strategy.hpp"
#include <string>
#include <vector>

using namespace std;

vector<csot::Tick> load_ticks(const string& path);

void replay_ticks(const vector<csot::Tick>& ticks,csot::Strategy& strategy);
