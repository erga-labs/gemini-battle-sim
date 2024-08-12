#include "castle.h"

void Castle::draw(Texture2D spritesheet)
{
    // Assuming width and height of the castle after scaling
    float castleWidth = 4.0f;
    float castleHeight = 4.0f; // Adjust this based on the combined height of both blocks

    // Calculate the position to center the castle in the bottom-right corner
    float posX = position.x - castleWidth / 2;
    float posY = position.y - castleHeight / 2;

    // Draw the upper block (Assume the height is half the total height)
    DrawTexturePro(spritesheet, Rectangle{0, 64, 32, 16}, Rectangle{posX, posY - castleHeight / 4, 4, 2}, Vector2{0, 0}, 0.0f, WHITE);

    // Draw the lower block
    DrawTexturePro(spritesheet, Rectangle{32, 64, 32, 16}, Rectangle{posX, posY + castleHeight / 4, 4, 2}, Vector2{0, 0}, 0.0f, WHITE);
}
