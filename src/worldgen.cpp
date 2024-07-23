
#include "worldgen.h"

std::vector<Tile> WorldGen::createWorld(int boundX, int boundY)
{
    // creating a vec of fixed size
    std::vector<Tile> tiles(boundX * boundY);

    for (int y = 0; y < boundY; y++)
    {
        for (int x = 0; x < boundX; x++)
        {
            tiles[x + y * boundY] = Tile::Grass;
        }
    }

    return tiles;
}

Texture WorldGen::createWorldTexture(int boundX, int boundY)
{
    // how crisp the texture is (16 is a good number for now)
    const float crispFactor = 16;

    Texture tiles[] = {
        LoadTexture("assets/grass.png"),
    };
    const int numTiles = sizeof(tiles) / sizeof(Texture);

    const std::vector<Tile> worldData = createWorld(boundX, boundY);
    // the world will be drawn into this
    RenderTexture renderTex = LoadRenderTexture(boundX * crispFactor, boundY * crispFactor);

    BeginTextureMode(renderTex);

    for (int y = 0; y < boundY; y++)
    {
        for (int x = 0; x < boundX; x++)
        {
            const Texture tex = tiles[(int)worldData[x + y * boundY]];
            const Rectangle srcRect = {0, 0, (float)tex.width, (float)tex.height};
            const Rectangle destRect = {x * crispFactor, y * crispFactor, crispFactor, crispFactor};
            const int rotation = GetRandomValue(0, 3);
            
            // the X origin changes when the angle is 180 or 270 degrees
            const float originX = (rotation == 2 || rotation == 3) ? 1 : 0;
            // the Y origin changes when the angle is 90 or 180 degrees
            const float originY = (rotation == 1 || rotation == 2) ? 1 : 0;
            const Vector2 origin = {originX * crispFactor, originY * crispFactor};

            const Color tintColors[] = {RED, BLUE, ORANGE, BROWN};
            DrawTexturePro(tex, srcRect, destRect, origin, rotation * 90, tintColors[rotation]);
        }
    }

    EndTextureMode();

    Texture out = renderTex.texture;
    renderTex.texture.id = 0;

    // unloading stuff
    for (int i = 0; i < numTiles; i++)
    {
        UnloadTexture(tiles[i]);
    }
    UnloadRenderTexture(renderTex);

    return out;
}
