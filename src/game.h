
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

private:
    // initializes the game
    void setup();
    // draws single frame
    void drawFrame();
    // handles inputs
    void processInputs();
    // draw the cloud map with some transparancy
    void drawCloud(uint8_t alpha);
    // draw the actual world
    void drawWorld();
    // draw the battalions
    void drawBattalions();

private:
    Camera2D m_camera;
    Vector2 m_worldBounds;

    std::vector<std::shared_ptr<Battalion>> m_battalions;

    float m_cloudDrawOffset = 0.0;
    // created by `WorldGen`
    Texture m_worldTexture, m_cloudTexture;
};
