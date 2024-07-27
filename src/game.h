
#pragma once

#include "src/battalion.h"
#include <raylib/raylib.h>
#include <vector>
#include <memory>

class Game
{

public:
    Game(int windowWidth, int windowHeight, const char *windowTitle = "Ant Simulation");
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
    void drawCloud(uint8_t alpha);
    void drawWorld();
    void drawBattalions();

private:
    Camera2D m_camera;
    Vector2 m_worldBounds;
    float m_cloudPos = 0.0;

    std::vector<std::shared_ptr<Battalion>> m_battalions;

    // created by `WorldGen`
    Texture m_worldTexture, m_cloudTexture;
};
