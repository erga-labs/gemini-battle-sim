
#include "battalion.h"


Battalion::Battalion(Vector2 initialPosition) {
    m_position = initialPosition;
    m_color = RED;
}


Battalion::~Battalion() {}


void Battalion::draw() {
    DrawEllipse(m_position.x, m_position.y, 3, 1, m_color);
}


void Battalion::setColor(Color color) {
    m_color = color;
}
