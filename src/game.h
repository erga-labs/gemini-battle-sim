
#pragma once

#include "src/battalionhandler.h"
#include <raylib/raylib.h>
#include <vector>
#include <memory>

class Game
{

public:
    enum class State
    {
        // waiting for the user to set the initial config
        WAITING_USER = 0,
        // waiting for gemini to set the initial config
        WAITING_GEMINI = 1,
        // simulation
        RUN_SIMULATION = 2,
        // simulation pause
        PAUSE_SIMULATION = 3,
    };

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

private:
    Camera2D m_camera;
    Vector2 m_worldBounds;
    State m_state = State::WAITING_USER;
    int m_targetFPS;

    BattalionHandler m_battalionHandler;

    float m_cloudDrawOffset = 0.0;
    // created by `WorldGen`
    Texture m_worldTexture, m_cloudTexture;
};
