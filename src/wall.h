#pragma once

struct Wall {
    Vector2 position;
    Vector2 size;  // Use size for width and height
    float health;

    // Constructor
    Wall(Vector2 pos, Vector2 sz, float hp = 100.0f) : position(pos), size(sz), health(hp) {}

    // Function to get the bounding box
    Rectangle getBoundingBox() const {
        return Rectangle{position.x , position.y , size.x, size.y};
    }
};