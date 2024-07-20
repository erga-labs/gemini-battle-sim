
#include "src/game.h"
#include <emscripten.h>
#include <raylib/raymath.h>


void emscriptenMainLoop(void* arg) {
    ((Game*)arg)->processFrame();
}


Game::Game(int windowWidth, int windowHeight, const char* windowTitle) {
    InitWindow(windowWidth, windowHeight, windowTitle);

    setup();
    loadAssets();
}


Game::~Game() {
    CloseWindow();
}


void Game::startGameLoop() {
    emscripten_set_main_loop_arg(emscriptenMainLoop, this, 0, 1);
}


void Game::processFrame() {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    processInputs();
    drawFrame();

    EndDrawing();
}


void Game::setup() {
    m_worldBounds = {160, 90};
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

    for (int i = 0; i < numBattalions; i++) {
        m_battalions.push_back(Battalion(positions[i]));
    }
}


void Game::loadAssets() {
}


void Game::drawFrame() {
    BeginMode2D(m_camera);
    drawWorld();
    drawBattalions();
    EndMode2D();

    DrawText(TextFormat("CamPos: %f %f", m_camera.target.x, m_camera.target.y), 10, 10, 20, BLACK);
    DrawText(TextFormat("CamScale: %f", m_camera.zoom), 10, 40, 20, BLACK);
}


void Game::processInputs() {
    processCameraInputs();
}


void Game::processCameraInputs() {

    const float camMoveSpeed = 10 / m_camera.zoom;

    if (IsKeyDown(KEY_W)) {
        m_camera.target.y -= camMoveSpeed;
    }
    if (IsKeyDown(KEY_A)) {
        m_camera.target.x -= camMoveSpeed;
    }
    if (IsKeyDown(KEY_S)) {
        m_camera.target.y += camMoveSpeed;
    }
    if (IsKeyDown(KEY_D)) {
        m_camera.target.x += camMoveSpeed;
    }

    const float zoomDelta = GetMouseWheelMove();
    m_camera.zoom = Clamp(m_camera.zoom + zoomDelta, 5, 15);

    m_camera.target = Vector2Clamp(m_camera.target, {0, 0}, m_worldBounds);
}


void Game::setBattalionColor(Color color) {
    for (Battalion& b : m_battalions) {
        b.setColor(color);
    }
}


void Game::drawWorld() {
    const Rectangle rec = {0, 0, m_worldBounds.x, m_worldBounds.y};
    const Color c1 = {255, 255, 255, 150};
    const Color c2 = {255, 0, 0, 150};
    const Color c3 = {0, 255, 0, 150};
    const Color c4 = {0, 0, 255, 150};
    DrawRectangleGradientEx(rec, c1, c2, c3, c4);
}


void Game::drawBattalions() {
    for (Battalion& b : m_battalions) {
        b.draw();
    }
}
