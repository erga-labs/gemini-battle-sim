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
    m_accuracy = (btype == BType::Archer) ? 0.6f : 0.6f; // 75% for Archers, 60% for Warriors
    m_cooldown = (btype == BType::Archer) ? 2.0f : 1.0f;

    if (group == Group::Defender)
    {
        m_color = (btype == BType::Archer) ? Color{60, 20, 220, 255} : Color{0, 0, 140, 255};
    }
    else
    {
        m_color = (btype == BType::Archer) ? Color{220, 20, 60, 255} : Color{140, 0, 0, 255};
    }
}

void Battalion::draw(bool selected) const
{
    const Rectangle rect = {m_position.x, m_position.y, m_currentTroopCount, 1.0};
    const Vector2 origin = {m_currentTroopCount / 2.0f, 0.5};

    DrawRectanglePro(rect, origin, m_rotation, m_color);

    // Drawing more visible attack and lookout ranges if the battalion is selected
    const uint8_t alpha = selected ? 30 : 5;
    DrawCircleV(m_position, m_attackRange, {0, 0, 255, alpha});
    DrawCircleV(m_position, m_lookoutRange, {0, 0, 255, alpha});
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
    if (m_cooldown > 0.0)
    {
        return;
    }

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

        m_cooldown = (m_btype == BType::Archer) ? 2.0f : 1.0f;
    }
}

void Battalion::update()
{
    m_cooldown -= GetFrameTime();
    attackTarget();
    moveTowardsTarget();
    rotateTowardsTarget();
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
        }
    }
}

void Battalion::rotateTowardsTarget()
{
    if (auto target = m_target.lock())
    {
        const Vector2 direction = Vector2Subtract(target->m_position, m_position);
        const float targetRotation = atan2f(direction.y, direction.x) * RAD2DEG + 90.0f;
        float deltaRotation = targetRotation - m_rotation;

        if (deltaRotation > 180.0f)
            deltaRotation -= 360.0f;
        else if (deltaRotation < -180.0f)
            deltaRotation += 360.0f;

        const float rotationStep = 60.0f * GetFrameTime();
        // m_rotation += std::copysign(rotationStep, deltaRotation);
        if (std::fabs(deltaRotation) <= rotationStep)
        {
            m_rotation = targetRotation; // If close enough, snap to target
        }
        else
        {
            m_rotation += std::copysign(rotationStep, deltaRotation);
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
