
#pragma once

#include "src/battalion.h"
#include <raylib/raylib.h>
#include <vector>


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
    void drawWorld();
    void drawBattalions();

private:
    Camera2D m_camera;
    Vector2 m_worldBounds;

    std::vector<Battalion> m_battalions;

};
