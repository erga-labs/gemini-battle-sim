
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
    WorldGen();
    ~WorldGen();
    Texture createWorldTexture(int boundX, int boundY);
    Texture createCloudTexture();

private:
    std::vector<Tile> createWorld(int boundX, int boundY);

private:
    Texture m_spriteSheet;
};
