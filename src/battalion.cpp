
#include "src/battalion.h"
#include <raylib/raymath.h>
#include <algorithm>

const float const_attackRange[] = {3.0f, 5.0f};
const float const_lookoutRange[] = {18.0f, 25.0f};
const float const_speed[] = {5.0f, 4.0f};
const float const_health[] = {30.0f, 15.0f};
const float const_damage[] = {10.0f, 5.0f};
const float const_accuracy[] = {0.8f, 0.6f};
const float const_cooldown[] = {0.5f, 1.0f};
const float const_rotation[] = {90.0f, 70.0f};

const Color const_colors[2][2] = {
    {Color{140, 0, 0, 255}, Color{220, 20, 60, 255}},
    {Color{0, 0, 140, 255}, Color{60, 20, 220, 255}},
};

Battalion::Battalion(int id, Group group, BType btype, Vector2 center, const std::vector<Vector2> troopOffsets)
    : m_id(id), m_group(group), m_btype(btype), m_center(center)
{
    m_rotation = 0.0;

    for (const Vector2 &offset : troopOffsets)
    {
        Troop troop = {
            .position = Vector2Add(m_center, offset),
            .health = const_health[(int)m_btype],
        };
        m_troops.push_back(troop);
    }
}

float Battalion::getActiveRatio(const Vector2 &position, float range) const
{
    const float rangeSqr = range * range;
    auto predicate = [&](const Troop &troop)
    {
        const float distSqr = Vector2DistanceSqr(troop.position, position);
        return distSqr < rangeSqr;
    };

    const int count = std::count_if(m_troops.begin(), m_troops.end(), predicate);
    return (float)count / getTroopCount();
}

float Battalion::getLookoutRatio() const
{
    if (auto target = m_target.lock())
    {
        return getActiveRatio(target->m_center, const_lookoutRange[(int)m_btype]);
    }
    return 0.0;
}

void Battalion::draw(bool selected) const
{
    const Color color = const_colors[(int)m_group][(int)m_btype];
    const uint8_t alpha = selected ? 30 : 10;

    const Rectangle rect = {m_center.x, m_center.y, (float)getTroopCount(), 1.0};
    const Vector2 origin = {(float)getTroopCount() / 2, 0.5};
    DrawRectanglePro(rect, origin, m_rotation, {color.r, color.g, color.b, alpha});

    for (const auto &troop : m_troops)
    {
        DrawCircleV(troop.position, 0.2, BLACK);
    }

    for (const auto &troop : m_troops)
    {
        DrawCircleV(troop.position, const_attackRange[(int)m_btype], {color.r, color.g, color.b, alpha});
    }
}

void Battalion::update(float deltaTime)
{
    m_cooldown -= deltaTime;
    removeDead();
    move(deltaTime);
    attack(deltaTime);
    rotate(deltaTime);
}

void Battalion::removeDead()
{
    auto predicate = [&](const Troop &troop)
    {
        return troop.health <= 0.0;
    };

    auto it = std::remove_if(m_troops.begin(), m_troops.end(), predicate);
    m_troops.erase(it, m_troops.end());
}

void Battalion::move(float deltaTime)
{
    if (auto target = m_target.lock())
    {
        // if `threshold` percent of troops can attack the target's center, dont move
        const float moveThreshold = 0.4;
        if (getActiveRatio(target->m_center, const_attackRange[(int)m_btype]) > moveThreshold)
        {
            return;
        }

        Vector2 movementVec = Vector2Subtract(target->m_center, m_center);
        movementVec = Vector2Normalize(movementVec);
        movementVec = Vector2Scale(movementVec, const_speed[(int)m_btype] * deltaTime);

        m_center = Vector2Add(m_center, movementVec);
        for (auto &troop : m_troops)
        {
            troop.position = Vector2Add(troop.position, movementVec);
        }
    }
}

void Battalion::attack(float deltaTime)
{
    if (m_cooldown > 0.0)
    {
        return;
    }

    if (auto target = m_target.lock())
    {
        // for each troop in the battalion, get the closest valid target's troop
        for (const Troop &troop : m_troops)
        {
            // getting the closesst troop
            Troop *targetTroop = nullptr;
            float closestDistSqr = std::numeric_limits<float>::max();
            for (auto &other : target->m_troops)
            {
                const float distSqr = Vector2DistanceSqr(troop.position, other.position);
                if (distSqr < closestDistSqr)
                {
                    closestDistSqr = distSqr;
                    targetTroop = &other;
                }
            }

            // trying to attack only if the closest troop is in attack range
            const float attackRangeSqr = const_attackRange[(int)m_btype] * const_attackRange[(int)m_btype];
            if (closestDistSqr < attackRangeSqr)
            {
                if ((float)rand() / RAND_MAX < const_accuracy[(int)m_btype])
                {
                    targetTroop->health -= const_damage[(int)m_btype];
                }
            }
        }

        m_cooldown = const_cooldown[(int)m_btype];
    }
}

void Battalion::rotate(float deltaTime)
{
    if (auto target = m_target.lock())
    {
        const Vector2 direction = Vector2Subtract(target->m_center, m_center);
        const float targetRotation = atan2f(direction.y, direction.x) * RAD2DEG;

        // making deltaRotation in range [-180, 180] degrees
        float deltaRotation = targetRotation - m_rotation + 90.0f;
        if (deltaRotation > 180.0f)
            deltaRotation -= 360.0f;
        else if (deltaRotation < -180.0f)
            deltaRotation += 360.0f;

        const float rotationStep = const_rotation[(int)m_btype] * deltaTime;
        m_rotation += std::copysign(rotationStep, deltaRotation);

        // Rotate each troop around the battalion center by the new rotation
        for (auto &troop : m_troops)
        {
            Vector2 relativePosition = Vector2Subtract(troop.position, m_center);
            relativePosition = Vector2Rotate(relativePosition, std::copysign(rotationStep, deltaRotation) * DEG2RAD);
            troop.position = Vector2Add(m_center, relativePosition);
        }
    }
}
