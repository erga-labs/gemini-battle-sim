
#include "src/game.h"
#include <emscripten.h>
#include <stdint.h>


Game* game = nullptr;


extern "C" {

// changing the battalion color from main.cpp is hard
// calls setBattalionColor on game -> world -> battalion

// when calling the function from js, prefix it with _
// _setColor
void setColor(uint8_t r, uint8_t g, uint8_t b) {
    game->setBattalionColor({
        .r = r,
        .g = g,
        .b = b,
        .a = 255,
    });
}

}


int main() {
    game = new Game(800, 450);
    game->startGameLoop();
}
