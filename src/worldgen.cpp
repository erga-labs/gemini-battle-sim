
#include "worldgen.h"


std::vector<Tile> WorldGen::createWorld(int boundX, int boundY) {
    // creating a vec of fixed size
    std::vector<Tile> tiles(boundX * boundY);

    for (int y = 0; y < boundY; y++) {
        for (int x = 0; x < boundX; x++) {
            tiles[x + y * boundY] = Tile::Grass;
        }
    }

    return tiles;
}


Texture WorldGen::createWorldTexture(int boundX, int boundY) {
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

    for (int y = 0; y < boundY; y++) {
        for (int x = 0; x < boundX; x++) {
            const Texture tex = tiles[(int) worldData[x + y * boundY]];
            const Rectangle srcRect = {0, 0, (float) tex.width, (float) tex.height};
            const Rectangle destRect = {x*crispFactor, y*crispFactor, crispFactor, crispFactor};
            DrawTexturePro(tex, srcRect, destRect, {0, 0}, 0, WHITE);
        }
    }

    EndTextureMode();

    Texture out = renderTex.texture;
    renderTex.texture.id = 0;

    // unloading stuff
    for (int i = 0; i < numTiles; i++) {
        UnloadTexture(tiles[i]);
    }
    UnloadRenderTexture(renderTex);

    return out;
}
