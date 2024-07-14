
#pragma once

#include "src/battalion.h"
#include <raylib/raylib.h>
#include <vector>


class World {

public:
    World() = default;
    // bounds is the overall bounding box for the world
    // if bounds = {20, 10}, the playing area is 200 cells
    World(Vector2 bounds);
    ~World();
    void draw(Camera2D gameCamera);

private:
    bool m_initialized = false;
    Vector2 m_bounds;

    std::vector<Battalion> m_battalions;

};
