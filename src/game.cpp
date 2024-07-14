
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


void Game::startGameLoop() { emscripten_set_main_loop_arg(emscriptenMainLoop, this, 0, 1); }


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

    m_gameWorld = World(m_worldBounds);
}


void Game::loadAssets() {
}


void Game::drawFrame() {
    m_gameWorld.draw(m_camera);

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
