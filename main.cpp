
#include "src/game.h"
#include <emscripten.h>
#include <stdint.h>

Game *game = nullptr;

extern "C"
{
    // when calling the function from js, prefix it with _
    // _setColor
    void setColor(uint8_t r, uint8_t g, uint8_t b)
    {
        TraceLog(LOG_WARNING, "This function has been removed");
    }
}

int main()
{
    game = new Game(800, 450);
    game->startGameLoop();
}
