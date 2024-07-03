
#pragma once

#include <raylib/raylib.h>


class Game {

public:
    Game(int windowWidth, int windowHeight, const char* windowTitle = "Ant Simulation");
    ~Game();
    void startGameLoop();
    void processFrame();

private:
    void loadAssets();
    void drawFrame();
    void processInputs();

private:
    Texture m_texture;
    float m_textureRotation = 0.0;

};
