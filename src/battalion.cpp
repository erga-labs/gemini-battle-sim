#include "battalion.h"

Battalion::Battalion(
    int id,
    Group group,
    BType btype,
    Vector2 position,
    int troopCount,
    float rotation) : m_id(id), m_group(group), m_btype(btype), m_position(position),
                      m_initialTroopCount(troopCount), m_currentTroopCount(troopCount),
                      m_rotation(rotation)
{
    // Derived values based on btype and troop count
    m_attackRange = (btype == BType::Archer) ? 5.0f : 3.0f;
    m_lookoutRange = (btype == BType::Archer) ? 25.0f : 17.5f;
    m_speed = 5.0f; // * (10.0f / float(troopCount));
    m_damage = (btype == BType::Archer) ? 10.0f : 15.0f;
    m_accuracy = (btype == BType::Archer) ? 0.6f : 0.75f; // 75% for Archers, 60% for Warriors
    m_cooldown = (btype == BType::Archer) ? 60 : 30;
}

void Battalion::draw(bool debug) const
{
    Color colorToDraw = debug ? RED : m_color;


    DrawRectanglePro(
        {m_position.x, m_position.y, float(m_currentTroopCount), 1}, // Rectangle
        {float(m_currentTroopCount) / 2.0f, 0.5f},                   // Origin point
        m_rotation,                                                  // Rotation angle
        colorToDraw);

    // Draw attack and lookout ranges
    DrawCircleV(m_position, m_attackRange, {0, 0, 255, 60});
    DrawCircleV(m_position, m_lookoutRange, {0, 0, 255, 40});

    // Draw debug info if enabled
    if (debug)
    {
        debugDraw();
    }
}

void Battalion::setColor(Color color)
{
    m_color = color;
}

void Battalion::debugDraw() const
{
    DrawText(TextFormat("Troops: %d", (int)m_currentTroopCount), m_position.x, m_position.y - 20, 10, WHITE);
}

bool Battalion::hasValidTarget() const
{
    if (auto target = m_target.lock())
    {
        float distance = Vector2Distance(m_position, target->m_position);
        return distance <= m_lookoutRange && m_currentTroopCount > 0;
    }
    return false;
}

void Battalion::setTarget(std::weak_ptr<Battalion> target)
{
    m_target = target;
}

void Battalion::attackTarget()
{

    if (auto target = m_target.lock())
    {
        float distance = Vector2Distance(m_position, target->m_position);
        if (distance <= m_attackRange)
        {
            // Calculate if the attack hits based on accuracy
            if ((float)rand() / RAND_MAX <= m_accuracy)
            {
                TraceLog(LOG_WARNING, "%d attacks %d", m_id, target->m_id);
                target->m_currentTroopCount -= 1.0f;
                if (target->m_currentTroopCount < 0)
                    target->m_currentTroopCount = 0;
            }
        }
    }
}

void Battalion::update()
{

    m_cooldown--;
    if (m_cooldown <= 0 && m_currentTroopCount > 0)
    {
        attackTarget();
        m_cooldown = (m_btype == BType::Archer) ? 60 : 30;
    }

    moveTowardsTarget();
    // enrage();
}

void Battalion::moveTowardsTarget()
{
    float deltaTime = GetFrameTime();

    if (auto target = m_target.lock())
    {
        float distance = Vector2Distance(m_position, target->m_position);
        if (distance <= m_lookoutRange)
        {
            if (distance <= m_attackRange)
            {
                return; // Do nothing, just attack
            }

            m_position = Vector2MoveTowards(m_position, target->m_position, m_speed * deltaTime);

            const Vector2 direction = Vector2Subtract(target->m_position, m_position);
            if (Vector2Length(direction) > 0.0)
            {
                m_rotation = atan2f(direction.y, direction.x) * RAD2DEG;
            }
        }
    }
}

void Battalion::enrage()
{
    if (m_currentTroopCount <= m_initialTroopCount * 0.9)
    {
        m_speed *= 1.1f;
        m_accuracy *= 1.1f;
    }
}
