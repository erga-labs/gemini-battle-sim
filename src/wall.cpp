#include "wall.h"

Rectangle Wall::getBoundingBox() const
{
    return Rectangle{position.x - size.x / 2, position.y - size.y / 2, size.x, size.y};
}

void Wall::setHP(float hp)
{
    Wall::health = hp;
}

void Wall::takeDamage(float damage)
{
    Wall::health -= damage;
}

float Wall::getHP()
{
    return Wall::health;
}

void Wall::draw(Texture2D spritesheet) const
{

    float baseX = 0;
    float baseY = 96;

    if (Wall::health < TOTAL_HEALTH * 0.66)
    {
        baseX = 32;
    }
    else if (Wall::health < TOTAL_HEALTH * 0.33)
    {
        baseX = 64;
    }

    Rectangle wallSourceRec = {baseX, baseY, 32, 16}; // Assuming wall sprite starts at 0,0 in the texture
    Rectangle wallDestRec = getBoundingBox();
    DrawTexturePro(spritesheet, wallSourceRec, wallDestRec, Vector2{0, 0}, 0.0f, WHITE);
}