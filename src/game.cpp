
#include "src/game.h"
#include <emscripten.h>


void emscriptenMainLoop(void* arg) {
    ((Game*) arg)->processFrame();
}


Game::Game(int windowWidth, int windowHeight, const char* windowTitle) {
    InitWindow(windowWidth, windowHeight, windowTitle);
    loadAssets();
}


Game::~Game() {
    UnloadTexture(m_texture);
    CloseWindow();
}


void Game::startGameLoop() {
    emscripten_set_main_loop_arg(emscriptenMainLoop, this, 0, 1);
}


void Game::processFrame() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    processInputs();
    drawFrame();

    EndDrawing();
}


void Game::loadAssets() {
    m_texture = LoadTexture("assets/placeholder.png");
}


void Game::drawFrame() {
    DrawText("Hold 'R' to change rotation", 10, 10, 20, BLACK);

    const Vector2 mousePosition = GetMousePosition();
    DrawTextureEx(m_texture, mousePosition, m_textureRotation, 1.0, WHITE);
}


void Game::processInputs() {
    if (IsKeyDown(KEY_R)) {
        m_textureRotation += 5.0;
    }
}
