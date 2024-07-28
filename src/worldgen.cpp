
#include "worldgen.h"

std::vector<Tile> WorldGen::createWorld(int boundX, int boundY)
{
    // creating a vec of fixed size
    std::vector<Tile> tiles(boundX * boundY);

    const int offset = GetRandomValue(0, 100) * 100;
    Image noiseImage = GenImagePerlinNoise(boundX, boundY, offset, offset, 3);
    Color *noiseData = (Color *)noiseImage.data;

    for (int y = 0; y < boundY; y++)
    {
        for (int x = 0; x < boundX; x++)
        {
            const float val = noiseData[x + y * boundX].r / 255.0f;
            Tile tile;
            if (val < 0.3)
            {
                tile = Tile::Dirt;
            }
            else if (val < 0.7)
            {
                tile = Tile::Weed;
            }
            else
            {
                tile = Tile::Grass;
            }
            tiles[x + y * boundX] = tile;
        }
    }

    UnloadImage(noiseImage);
    return tiles;
}

Texture WorldGen::createWorldTexture(int boundX, int boundY)
{
    // how crisp the texture is (16 is a good number for now)
    const float crispFactor = 16;

    Texture spriteSheet = LoadTexture("assets/spritesheet-ai.png");
    SetTextureFilter(spriteSheet, TEXTURE_FILTER_POINT);

    // dirt texture: 0 to 1
    // weed texture: 2 to 3
    // grass texture: 4 to 5
    // grass overlay: 6
    const Rectangle srcRects[] = {
        {0, 32, 16, 16},
        {16, 32, 16, 16},
        {0, 16, 16, 16},
        {16, 16, 16, 16},
        {0, 0, 16, 16},
        {16, 0, 16, 16},
        {0, 48, 16, 16},
    };

    const std::vector<Tile> worldData = createWorld(boundX, boundY);
    // the world will be drawn into this
    RenderTexture renderTex = LoadRenderTexture(boundX * crispFactor, boundY * crispFactor);

    BeginTextureMode(renderTex);

    for (int y = 0; y < boundY; y++)
    {
        for (int x = 0; x < boundX; x++)
        {
            // const Tile tile = worldData[x + y * boundX];
            // const int texIndex = GetRandomValue((int)tile * 2, (int)tile * 2 + 1);
            // const Texture tex = assets[texIndex];
            // const Rectangle srcRect = {0, 0, (float)tex.width, (float)tex.height};
            // const Rectangle destRect = {x * crispFactor, y * crispFactor, crispFactor, crispFactor};
            // DrawTexturePro(tex, srcRect, destRect, {0, 0}, 0, WHITE);

            const Tile tile = worldData[x + y * boundX];
            const int texIndex = GetRandomValue((int)tile * 2, (int)tile * 2 + 1);
            const Rectangle destRect = {x * crispFactor, y * crispFactor, crispFactor, crispFactor};
            DrawTexturePro(spriteSheet, srcRects[texIndex], destRect, {0, 0}, 0, WHITE);

            if (tile == Tile::Dirt)
            {
                const int offsets[4][2] = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}};
                for (int i = 0; i < 4; i++)
                {
                    const int dx = x - offsets[i][0];
                    const int dy = y - offsets[i][1];
                    if (dx < 0 || dx > boundX || dy < 0 || dy > boundY || worldData[dx + dy * boundX] == Tile::Dirt)
                    {
                        continue;
                    }

                    const int quads[4] = {3, 2, 0, 1};
                    const int quad = quads[i];
                    // the X origin changes when the angle is 180 or 270 degrees
                    const float originX = (quad == 2 || quad == 3) ? 1 : 0;
                    // the Y origin changes when the angle is 90 or 180 degrees
                    const float originY = (quad == 1 || quad == 2) ? 1 : 0;
                    const Vector2 origin = {originX * crispFactor, originY * crispFactor};

                    DrawTexturePro(spriteSheet, srcRects[6], destRect, origin, quad * 90, {255, 255, 255, 235});
                }
            }
        }
    }

    EndTextureMode();

    Texture out = renderTex.texture;
    SetTextureFilter(out, TEXTURE_FILTER_POINT);

    // unloading stuff
    UnloadTexture(spriteSheet);
    renderTex.texture.id = 0;
    UnloadRenderTexture(renderTex);

    return out;
}
