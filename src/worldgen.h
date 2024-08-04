
#pragma once

#include <raylib/raylib.h>
#include <vector>

enum class Tile
{
    Dirt = 0,
    Weed = 1,
    Grass = 2,
};

class WorldGen
{

public:
    static Texture createWorldTexture(int boundX, int boundY);
    static Texture createCloudTexture();

private:
    static std::vector<Tile> createWorld(int boundX, int boundY);
};
