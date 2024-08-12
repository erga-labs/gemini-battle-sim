#pragma once

#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <algorithm>

struct Castle {
    Vector2 position;
    float health;

    Castle(Vector2 position, float health) : position(position), health(health) {}

    void draw(Texture2D spritesheet);

    void takeDamage(float damage) {
         health -= damage; 
    }

};