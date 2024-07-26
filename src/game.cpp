
#include "src/game.h"
#include "src/worldgen.h"
#include <emscripten.h>
#include <raylib/raymath.h>

void emscriptenMainLoop(void *arg)
{
    ((Game *)arg)->processFrame();
}

Game::Game(int windowWidth, int windowHeight, const char *windowTitle)
{
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, windowTitle);

    setup();
    loadAssets();
}

Game::~Game()
{
    UnloadTexture(m_worldTexture);
    CloseWindow();
}

void Game::startGameLoop()
{
    emscripten_set_main_loop_arg(emscriptenMainLoop, this, 0, 1);
}

void Game::processFrame()
{
    BeginDrawing();
    ClearBackground(DARKGRAY);

    processInputs();
    drawFrame();

    EndDrawing();
}

void Game::setup()
{
    m_worldBounds = {80, 45};
    m_camera = {
        .offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
        .target = {m_worldBounds.x / 2, m_worldBounds.y / 2},
        .rotation = 0.0,
        .zoom = 10.0,
    };

    const Vector2 positions[] = {
        {m_worldBounds.x / 2, m_worldBounds.y / 2},
        {m_worldBounds.x / 2, (m_worldBounds.y - 20) / 2},
    };
    const int numBattalions = sizeof(positions) / sizeof(Vector2);

    for (int i = 0; i < numBattalions; i++)
    {
        m_battalions.push_back(Battalion(std::weak_ptr<Battalion>(), 0, 0, 0, positions[i], 5, 0, B_Type::Warrior));
    }

    m_worldTexture = WorldGen::createWorldTexture(m_worldBounds.x, m_worldBounds.y);
}

void Game::loadAssets()
{
}

void Game::drawFrame()
{
    BeginMode2D(m_camera);
    drawWorld();
    drawBattalions();
    EndMode2D();

    const Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), m_camera);
    DrawText(TextFormat("MousePos: %f %f", mousePos.x, mousePos.y), 10, 10, 20, BLACK);
    DrawText(TextFormat("CamScale: %f", m_camera.zoom), 10, 40, 20, BLACK);
}

void Game::processInputs()
{
    processCameraInputs();
}

void Game::processCameraInputs()
{

    const float camMoveSpeed = 10 / m_camera.zoom;

    if (IsKeyDown(KEY_W))
    {
        m_camera.target.y -= camMoveSpeed;
    }
    if (IsKeyDown(KEY_A))
    {
        m_camera.target.x -= camMoveSpeed;
    }
    if (IsKeyDown(KEY_S))
    {
        m_camera.target.y += camMoveSpeed;
    }
    if (IsKeyDown(KEY_D))
    {
        m_camera.target.x += camMoveSpeed;
    }

    const float zoomDelta = GetMouseWheelMove();
    const float minZoom = 15;
    const float maxZoom = 30;
    m_camera.zoom = Clamp(m_camera.zoom + zoomDelta, minZoom, maxZoom);

    Vector2 camPadding = {15, 15};
    camPadding = Vector2Scale(camPadding, 1 / m_camera.zoom);
    camPadding = Vector2Multiply(camPadding, {16, 9});
    const Vector2 minCamPos = camPadding;
    const Vector2 maxCamPos = Vector2Subtract(m_worldBounds, camPadding);
    m_camera.target = Vector2Clamp(m_camera.target, minCamPos, maxCamPos);
}

void Game::setBattalionColor(Color color)
{
    for (Battalion &b : m_battalions)
    {
        b.setColor(color);
    }
}

void Game::drawWorld()
{
    const Rectangle srcRect = {0, 0, (float)m_worldTexture.width, (float)m_worldTexture.height};
    const Rectangle destRect = {0, 0, m_worldBounds.x, m_worldBounds.y};
    const Vector2 origin = {m_worldBounds.x, m_worldBounds.y};
    DrawTexturePro(m_worldTexture, srcRect, destRect, origin, 180, WHITE);
}

void Game::drawBattalions()
{
    for (Battalion &b : m_battalions)
    {
        b.draw(m_camera);
    }
}
