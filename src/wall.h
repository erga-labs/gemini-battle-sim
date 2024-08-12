#pragma once

struct Wall
{
    Vector2 position;
    Vector2 size; // Use size for width and height
    float health;

    // Constructor
    Wall(Vector2 pos, Vector2 sz, float hp = 500.0f) : position(pos), size(sz), health(hp) {}

    // Function to get the bounding box
    Rectangle getBoundingBox() const
    {
        return Rectangle{position.x - size.x / 2, position.y - size.y / 2, size.x, size.y};
    }
};