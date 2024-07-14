
#include "src/world.h"
#include <raylib/raymath.h>


World::World(Vector2 bounds) {
    m_initialized = true;
    m_bounds = bounds;

    const Vector2 positions[] = {
        {bounds.x / 2, bounds.y / 2},
        {bounds.x / 2, (bounds.y - 20) / 2},
    };
    const int numBattalions = sizeof(positions) / sizeof(Battalion);

    for (int i = 0; i < numBattalions; i++) {
        m_battalions.push_back(Battalion(positions[i]));
    }
}


World::~World() {
    if (!m_initialized) {
        return;
    }

    // destructor code here
}


void World::draw(Camera2D gameCamera) {
    BeginMode2D(gameCamera);

    // draws the entire play area
    // DrawRectangle(0, 0, m_bounds.x, m_bounds.y, {255, 0, 0, 50});
    DrawRectangleGradientEx({0, 0, m_bounds.x, m_bounds.y}, {255, 255, 255, 150}, {255, 0, 0, 150},
                            {0, 255, 0, 150}, {0, 0, 255, 150});

    for (Battalion& b : m_battalions) {
        b.draw();
    }

    EndMode2D();
}
