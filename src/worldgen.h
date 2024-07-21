
#pragma once

#include <raylib/raylib.h>
#include <vector>


enum class Tile {
    Grass = 0,
};


class WorldGen {

public:
    static std::vector<Tile> createWorld(int boundX, int boundY);
    static Texture createWorldTexture(int boundX, int boundY);

};
