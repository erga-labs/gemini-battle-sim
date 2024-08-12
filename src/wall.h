#pragma once
#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <algorithm>

#define TOTAL_HEALTH 100.0f

struct Wall
{
    Vector2 position, size;
    // Use size for width and height
    static float health;

    // Constructor
    Wall(Vector2 pos, Vector2 sz) : position(pos), size(sz) { Wall::setHP(TOTAL_HEALTH); }

    // Function to set the health
    static void takeDamage(float damage);
    static float getHP();
    static void setHP(float hp);
    Rectangle getBoundingBox() const;
    void draw(Texture2D spritesheet) const;
    // Function to get the bounding box
};