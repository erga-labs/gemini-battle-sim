
#pragma once

#include <raylib/raylib.h>
#include <vector>

struct BattalionSpawnInfo
{
    int id;
    int btype;
    std::vector<Vector2> troops;
};