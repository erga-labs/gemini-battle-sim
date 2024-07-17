
#pragma once

#include "src/world.h"
#include <raylib/raylib.h>


class Game {

public:
    Game(int windowWidth, int windowHeight, const char* windowTitle = "Ant Simulation");
    ~Game();
    void startGameLoop();
    void processFrame();
    void setBattalionColor(Color color);

private:
    void setup();
    void loadAssets();
    void drawFrame();
    void processInputs();
    void processCameraInputs();

private:
    Camera2D m_camera;
    Vector2 m_worldBounds;

    World m_gameWorld;
};
