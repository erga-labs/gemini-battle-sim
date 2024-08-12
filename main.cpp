
#include "src/game.h"
#include <emscripten.h>
#include <stdint.h>

Game *game = nullptr;

int main()
{
    game = new Game(1280, 720, "Battle Simulation w/ Gemini");
    game->startGameLoop();
}
