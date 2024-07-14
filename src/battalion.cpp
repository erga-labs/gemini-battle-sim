
#include "battalion.h"


Battalion::Battalion(Vector2 initialPosition) {
    m_position = initialPosition;
}


Battalion::~Battalion() {}


void Battalion::draw() {
    DrawEllipse(m_position.x, m_position.y, 3, 1, RED);
}
