#pragma once

#include <vector>

using namespace std;

struct room {
    int right = 0;
    int bottom = 0;

    int s = 0;
};

extern vector<vector<room>> maze;

void makeMaze(int n, int m);