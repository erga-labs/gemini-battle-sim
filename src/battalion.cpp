#include "battalion.h"

Battalion::Battalion(
    std::weak_ptr<Battalion> target,
    int group,
    float aggression,
    float range,
    Vector2 pos,
    int init_size,
    float damage,
    B_Type type) : m_target(target), m_group(group), m_aggression(aggression),
                   m_range(range), m_position(pos), m_size(init_size),
                   m_damage(damage), m_type(type), m_debugColor(BLUE),
                   m_rotation(0.0f) // Initialize rotation angle to 0
{
}

Battalion::~Battalion() {}

void Battalion::draw()
{
    Color colorToDraw = isMouseHover() ? m_debugColor : Color{m_debugColor.r, m_debugColor.g, m_debugColor.b, 0}; // Transparent if not hovered
    DrawRectanglePro(
        {m_position.x, m_position.y, float(m_size), float(m_size)}, // Rectangle
        {m_size / 2.0f, m_size / 2.0f},
        m_rotation, // Rotation angle
        colorToDraw);
}

void Battalion::setDebugColor(Color color)
{
    m_debugColor = color;
}

bool Battalion::isMouseHover() const
{
    Vector2 mousePosition = GetMousePosition();
    return CheckCollisionPointRec(mousePosition, {m_position.x, m_position.y, float(m_size), float(m_size)});
}

void Battalion::update()
{
    checkTarget();
    move();
    attack();
    enrage();
}

void Battalion::enrage()
{
    m_aggression *= 1.1f;
}

void Battalion::attack()
{
    if (auto target = m_target.lock())
    {
        float distance = Vector2Distance(m_position, target->m_position);
        if (distance <= m_range)
        {
            target->m_size -= m_damage;
        }
    }
}

void Battalion::move()
{
    if (auto target = m_target.lock())
    {
        Vector2 direction = Vector2Subtract(target->m_position, m_position);
        if (Vector2Length(direction) > 0)
        {
            direction = Vector2Normalize(direction);
            m_position = Vector2Add(m_position, Vector2Scale(direction, m_aggression));

            m_rotation = atan2f(direction.y, direction.x) * RAD2DEG;
        }
    }
}

void Battalion::checkTarget()
{
    if (auto target = m_target.lock())
    {
        float distance = Vector2Distance(m_position, target->m_position);
        if (distance > m_range)
        {
            // Out of Range Logic
        }
    }
    else
    {
        // Change Target to the palace.
    }
}

void Battalion::setColor(Color color)
{
    m_debugColor = color;
}