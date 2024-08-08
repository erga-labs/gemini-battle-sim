
#pragma once

#include <raylib/raylib.h>
#include <vector>

struct BattalionSpawnInfo
{
    int id;
    Vector2 position;
    int btype;
    std::vector<Vector2> troops;
};