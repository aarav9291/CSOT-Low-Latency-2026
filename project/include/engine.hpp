#pragma once
#include "strategy.hpp"
#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace std;

class Engine {
public:
    vector<csot::Tick> load_ticks(const string& path);

private:
    deque<string> symbol_storage_;
    unordered_map<string, string_view> interned_;
};
