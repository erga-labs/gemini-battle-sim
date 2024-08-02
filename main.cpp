
#include "src/game.h"
#include <emscripten.h>
#include <stdint.h>

Game *game = nullptr;

extern "C"
{
    // functions to be exposed here
}

int main()
{
    game = new Game(800, 450);
    game->startGameLoop();
}
