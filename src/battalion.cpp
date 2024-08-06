
#include "src/battalion.h"
#include <raylib/raymath.h>
#include <algorithm>

/// Constants defination

const float const_attackRange[] = {3.0f, 5.0f};
const float const_lookoutRange[] = {18.0f, 25.0f};
const float const_speed[] = {5.0f, 4.0f};
const float const_health[] = {30.0f, 15.0f};
const float const_damage[] = {25.0f, 15.0f};
const float const_accuracy[] = {0.8f, 0.6f};
const float const_cooldown[] = {0.5f, 1.0f};
const float const_rotation[] = {90.0f, 70.0f};

const Color colors[2][2] = {
    {Color{0, 0, 140, 255}, Color{60, 20, 220, 255}},
    {Color{140, 0, 0, 255}, Color{220, 20, 60, 255}},
};

/// Class defination

Battalion::Battalion(int id, Group group, BType btype, Vector2 center, const std::vector<Vector2> &troopPositions)
    : m_id(id), m_group(group), m_btype(btype), m_center(center)
{
    for (const Vector2 &pos : troopPositions)
    {
        const Troop troop = {
            .position = Vector2Add(center, pos),
            .currHealth = const_health[(int)m_btype],
        };
        m_troops.push_back(troop);
    }

    m_cooldown = const_cooldown[(int)m_btype];
}

float Battalion::hasValidTarget() const
{
    if (auto target = m_target.lock())
    {
        const float range = const_lookoutRange[(int)m_btype];
        const float activeRatio = getActiveRatio(target->m_center, range);
        return activeRatio;
    }

    return 0.0f;
}

void Battalion::draw(bool selected) const
{
    DrawCircleV(m_center, 0.8f, BLACK);
    for (const auto &troop : m_troops)
    {
        DrawCircleV(troop.position, 0.4f, colors[(int)m_group][(int)m_btype]);

        const Color color = colors[(int)m_group][(int)m_btype];
        DrawCircleV(troop.position, const_attackRange[(int)m_btype], {color.r, color.g, color.b, 40});
    }

    // Drawing more visible attack and lookout ranges if the battalion is selected
    const uint8_t alpha = selected ? 30 : 20;
    DrawCircleV(m_center, const_attackRange[(int)m_btype], {0, 0, 255, alpha});
    // DrawCircleV(m_center, const_lookoutRange[(int) m_btype], {0, 0, 255, alpha});
}

void Battalion::update()
{
    m_cooldown -= GetFrameTime();
    removeDead();
    move();
    attack();
    rotate();
}

float Battalion::getActiveRatio(const Vector2 &position, float range) const
{
    const float rangeSqr = range * range;
    auto predicate = [&](const Troop &troop)
    {
        return Vector2DistanceSqr(troop.position, position) < rangeSqr;
    };

    const int active = std::count_if(m_troops.begin(), m_troops.end(), predicate);
    return (float)active / getTroopCount();
}

float Battalion::getFactor() const
{
    /*
        returns 1 if troopCount in ( 1..14)
        returns 2 if troopCount in (15..29)
    */
    const int troopCount = getTroopCount();
    return troopCount / 15 + 1.0;
}

void Battalion::move()
{
    if (auto target = m_target.lock())
    {
        const float deltaTime = GetFrameTime();
        const float aRange = const_attackRange[(int)m_btype];
        const float aThreshold = 0.4;
        const float activeRatio = getActiveRatio(target->m_center, aRange);

        if (activeRatio > aThreshold)
        {
            return;
        }

        const float frameSpeed = const_speed[(int)m_btype] / getFactor() * deltaTime;
        Vector2 movementVec = Vector2Subtract(target->m_center, m_center);
        movementVec = Vector2Normalize(movementVec);
        movementVec = Vector2Scale(movementVec, frameSpeed);

        m_center = Vector2Add(m_center, movementVec);
        for (auto &troop : m_troops)
        {
            troop.position = Vector2Add(troop.position, movementVec);
        }
    }
}

void Battalion::attack()
{
    if (m_cooldown > 0.0f)
    {
        return;
    }

    if (auto target = m_target.lock())
    {
        for (const auto &troop : m_troops)
        {
            Troop &otherTroop = target->m_troops[0];
            float closest = std::numeric_limits<float>::max();

            for (auto &other : target->m_troops)
            {
                const float distanceSqr = Vector2DistanceSqr(troop.position, other.position);
                if (distanceSqr < closest)
                {
                    closest = distanceSqr;
                    otherTroop = other;
                }
            }

            const float attackRangeSqr = const_attackRange[(int)m_btype] * const_attackRange[(int)m_btype];
            if (closest < attackRangeSqr)
            {
                if ((float)rand() / RAND_MAX < const_accuracy[(int)m_btype])
                {
                    otherTroop.currHealth -= const_damage[(int)m_btype];
                    TraceLog(LOG_WARNING, "attack | distanceSqr: %f attackRangeSqr: %f", closest, attackRangeSqr);
                }
            }
        }

        m_cooldown = const_cooldown[(int)m_btype];
    }
}

void Battalion::rotate()
{
    if (auto target = m_target.lock())
    {
        const Vector2 direction = Vector2Subtract(target->m_center, m_center);
        const float targetRotation = atan2f(direction.y, direction.x) * RAD2DEG + 90.0f;
        float deltaRotation = targetRotation - m_rotation;

        if (deltaRotation > 180.0f)
            deltaRotation -= 360.0f;
        else if (deltaRotation < -180.0f)
            deltaRotation += 360.0f;

        const float rotationStep = const_rotation[(int)m_btype] * GetFrameTime();

        // if deltaRotation is really small, then snap to targetRotation
        if (fabs(deltaRotation) < rotationStep)
        {
            m_rotation = targetRotation;
        }
        else
        {
            m_rotation += std::copysign(rotationStep, deltaRotation);
            for (auto &troop : m_troops)
            {
                Vector2 v = Vector2Subtract(troop.position, m_center);
                v = Vector2Rotate(v, deltaRotation);
                troop.position = Vector2Add(v, m_center);
            }
        }
    }
}

void Battalion::removeDead()
{
    auto predicate = [&](const Troop &troop)
    {
        return troop.currHealth < 0.0f;
    };

    auto it = std::remove_if(m_troops.begin(), m_troops.end(), predicate);
    m_troops.erase(it, m_troops.end());
}
