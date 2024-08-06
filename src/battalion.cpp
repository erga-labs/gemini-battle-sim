
#include "src/battalion.h"
#include <raylib/raymath.h>
#include <algorithm>

/// Constants defination

#define RMAX 2147483647.0f

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
    const Rectangle rect = {m_center.x, m_center.y, (float)getTroopCount(), 1.0};
    const Vector2 origin = {(float)getTroopCount() / 2.0f, 0.5};

    DrawRectanglePro(rect, origin, m_rotation, BLUE);
    DrawCircleV(m_center, 0.8f, BLACK);
    for (const auto &troop : m_troops)
    {
        DrawCircleV(troop.position, 0.4f, colors[(int)m_group][(int)m_btype]);
        DrawCircleV(troop.position, const_attackRange[(int)m_btype], {40, 40, 40, 60});
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
    // rotate();
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
        // Calculate the direction vector to the target
        Vector2 movementVec = Vector2Subtract(target->m_center, m_center);
        float distanceToTarget = Vector2Length(movementVec);

        // Define a small threshold distance (dead zone) to prevent minor movements
        const float deadZoneThreshold = 0.5f;
        if (distanceToTarget < deadZoneThreshold)
        {
            // If within dead zone, don't move
            return;
        }

        // Normalize movement vector and scale by speed
        movementVec = Vector2Normalize(movementVec);
        const float deltaTime = GetFrameTime();
        const float frameSpeed = const_speed[(int)m_btype] * deltaTime;
        movementVec = Vector2Scale(movementVec, frameSpeed);

        // Calculate new center position
        Vector2 newCenter = Vector2Add(m_center, movementVec);

        // Calculate the target rotation angle in degrees
        float targetRotation = atan2f(movementVec.y, movementVec.x) * RAD2DEG + 90.0f; // +90 degrees to face "| vs |"

        // Normalize the rotation angle to [-180, 180] degrees
        if (targetRotation > 180.0f)
            targetRotation -= 360.0f;
        else if (targetRotation < -180.0f)
            targetRotation += 360.0f;

        // Apply the rotation to each troop's relative position
        float rotationAngle = targetRotation - m_rotation;
        if (rotationAngle > 180.0f)
            rotationAngle -= 360.0f;
        else if (rotationAngle < -180.0f)
            rotationAngle += 360.0f;

        // Update troop positions based on the new center and relative positions
        for (auto &troop : m_troops)
        {
            // Calculate the relative position of the troop to the battalion center
            Vector2 relativePosition = Vector2Subtract(troop.position, m_center);
            // Rotate the relative position
            relativePosition = Vector2Rotate(relativePosition, rotationAngle * DEG2RAD);
            // Update troop position with the new center position and rotated relative position
            troop.position = Vector2Add(newCenter, relativePosition);
        }

        // Update the battalion center position and rotation
        m_center = newCenter;
        m_rotation = targetRotation;
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
        for (auto &troop : m_troops)
        {
            Troop *closestTroop = nullptr;
            float closestDistanceSquared = RMAX;

            for (auto &other : target->m_troops)
            {
                const float distanceSquared = Vector2DistanceSqr(troop.position, other.position);
                if (distanceSquared < closestDistanceSquared)
                {
                    closestDistanceSquared = distanceSquared;
                    closestTroop = &other;
                }
            }

            float attackRangeSquared = const_attackRange[(int)m_btype] * const_attackRange[(int)m_btype];
            if (closestDistanceSquared <= attackRangeSquared)
            {
                if ((float)rand() / (float)RAND_MAX < const_accuracy[(int)m_btype])
                {
                    TraceLog(LOG_WARNING, "Battalion %d attacked Battalion %d", m_id, target->m_id);
                    TraceLog(LOG_WARNING, "Battalion Distance %f (attackRange: %f)", sqrtf(closestDistanceSquared), const_attackRange[(int)m_btype]);
                    closestTroop->currHealth -= const_damage[(int)m_btype];
                }
            }
        }

        m_cooldown = const_cooldown[(int)m_btype];
    }
}

// void Battalion::rotate()
// {
//     if (auto target = m_target.lock())
//     {
//         // Calculate the direction vector from this battalion's center to the target's center
//         Vector2 direction = Vector2Subtract(target->m_center, m_center);
//         // Calculate the angle in degrees
//         float targetRotation = atan2f(direction.y, direction.x) * RAD2DEG;

//         // Adjust the targetRotation to make it perpendicular (90 degrees offset)
//         targetRotation += 90.0f;

//         // Normalize targetRotation to the range [-180, 180] degrees
//         if (targetRotation > 180.0f)
//             targetRotation -= 360.0f;
//         else if (targetRotation < -180.0f)
//             targetRotation += 360.0f;

//         // Calculate the difference in rotation needed (deltaRotation)
//         float deltaRotation = targetRotation - m_rotation;
//         if (deltaRotation > 180.0f)
//             deltaRotation -= 360.0f;
//         else if (deltaRotation < -180.0f)
//             deltaRotation += 360.0f;

//         // Define a threshold to stop adjusting rotation when close enough
//         const float rotationThreshold = 1.0f; // degrees
//         const float rotationStep = const_rotation[(int)m_btype] * GetFrameTime();

//         // Apply the rotation, ensuring it doesn't overshoot the target
//         if (fabs(deltaRotation) > rotationThreshold)
//         {
//             if (fabs(deltaRotation) > rotationStep)
//             {
//                 m_rotation += std::copysign(rotationStep, deltaRotation);
//             }
//             else
//             {
//                 m_rotation = targetRotation;
//             }

//             // Rotate each troop around the battalion center by the new rotation
//             for (auto &troop : m_troops)
//             {
//                 Vector2 relativePosition = Vector2Subtract(troop.position, m_center);
//                 relativePosition = Vector2Rotate(relativePosition, deltaRotation * DEG2RAD);
//                 troop.position = Vector2Add(m_center, relativePosition);
//             }
//         }
//     }
// }


void Battalion::removeDead()
{
    auto predicate = [&](const Troop &troop)
    {
        return troop.currHealth < 0.0f;
    };

    auto it = std::remove_if(m_troops.begin(), m_troops.end(), predicate);
    m_troops.erase(it, m_troops.end());
}
