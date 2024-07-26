
#include "worldgen.h"

std::vector<Tile> WorldGen::createWorld(int boundX, int boundY)
{
    // creating a vec of fixed size
    std::vector<Tile> tiles(boundX * boundY);

    Image noiseImage = GenImagePerlinNoise(boundX, boundY, 0, 0, 2);
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

    Texture assets[] = {
        LoadTexture("assets/dirt_1.png"),
        LoadTexture("assets/dirt_2.png"),
        LoadTexture("assets/weeded_grass_1.png"),
        LoadTexture("assets/weeded_grass_2.png"),
        LoadTexture("assets/grass_1.png"),
        LoadTexture("assets/grass_2.png"),
    };
    const int numAssets = sizeof(assets) / sizeof(Texture);

    const std::vector<Tile> worldData = createWorld(boundX, boundY);
    // the world will be drawn into this
    RenderTexture renderTex = LoadRenderTexture(boundX * crispFactor, boundY * crispFactor);

    BeginTextureMode(renderTex);

    for (int y = 0; y < boundY; y++)
    {
        for (int x = 0; x < boundX; x++)
        {
            const Tile tile = worldData[x + y * boundX];
            const int texIndex = GetRandomValue((int)tile * 2, (int)tile * 2 + 1);
            const Texture tex = assets[texIndex];
            const Rectangle srcRect = {0, 0, (float)tex.width, (float)tex.height};
            const Rectangle destRect = {x * crispFactor, y * crispFactor, crispFactor, crispFactor};
            // const int rotation = GetRandomValue(0, 3);
            const int rotation = 0;

            // the X origin changes when the angle is 180 or 270 degrees
            const float originX = (rotation == 2 || rotation == 3) ? 1 : 0;
            // the Y origin changes when the angle is 90 or 180 degrees
            const float originY = (rotation == 1 || rotation == 2) ? 1 : 0;
            const Vector2 origin = {originX * crispFactor, originY * crispFactor};

            DrawTexturePro(tex, srcRect, destRect, origin, rotation * 90, WHITE);
        }
    }

    EndTextureMode();

    Texture out = renderTex.texture;
    renderTex.texture.id = 0;

    // unloading stuff
    for (int i = 0; i < numAssets; i++)
    {
        UnloadTexture(assets[i]);
    }
    UnloadRenderTexture(renderTex);

    return out;
}

Texture WorldGen::createCloudTexture()
{
    // NOTE: Maybe have an actual cloud asset
    Image cloudImage = GenImageWhiteNoise(32, 16, 0.3);
    Color *data = (Color *)cloudImage.data;
    const Color cloudColor = ColorBrightness(SKYBLUE, -0.1);

    for (int i = 0; i < cloudImage.width * cloudImage.height; i++)
    {
        data[i].a = data[i].r;
        if (data[i].a == 255)
        {
            data[i] = cloudColor;
        }
    }

    Texture out = LoadTextureFromImage(cloudImage);
    SetTextureWrap(out, TEXTURE_WRAP_REPEAT);

    UnloadImage(cloudImage);
    return out;
}
