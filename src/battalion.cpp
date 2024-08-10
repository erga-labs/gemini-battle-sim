
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

Battalion::Battalion(int id, Group group, BType btype, const std::vector<Vector2> troopPositions)
    : m_id(id), m_group(group), m_btype(btype)
{
    m_rotation = 0.0;

    Vector2 sum = {0.0f, 0.0f};
    for (const Vector2 &position : troopPositions)
    {
        Troop troop = {
            .position = position,
            .health = const_health[(int)m_btype],
            .currentFrame = 0,
            .state = IDLE,
            .frameCounter = 0.0f,
            .flipHorizontal = false,
        };
        m_troops.push_back(troop);
        sum = Vector2Add(sum, position);
    }

    m_initialTroopCount = getTroopCount();
    m_center = Vector2Scale(sum, 1.0f / m_initialTroopCount);
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

Rectangle GetFrameRectangle(int startX, int startY, int frameWidth, int frameHeight, int frameIndex)
{
    return Rectangle{(float)(startX + frameIndex * frameWidth), (float)startY, (float)frameWidth, (float)frameHeight};
}

int GetStartingYPosition(Group group, BType btype, TroopState state)
{
    int baseY = 0;

    if (group == Group::Attacker)
    {
        baseY = 48;
    }
    else if (group == Group::Defender)
    {
        baseY = 208;
    }

    switch (state)
    {
    case MOVING:
        return baseY;
    case MOVING_UP:
        return baseY + 16;
    case MOVING_DOWN:
        return baseY + 32;
    case ATTACKING:
        return baseY + 48;
    case ATTACKING_DOWN:
        return baseY + 64;
    case ATTACKING_UP:
        return baseY + 80;
    default:
        return baseY;
    }
}

void Battalion::draw(bool selected, Texture2D spritesheet) const
{
    const Color color = const_colors[(int)m_group][(int)m_btype];
    const uint8_t alpha = selected ? 30 : 10;

    const Rectangle rect = {m_center.x, m_center.y, (float)getTroopCount(), 1.0};
    const Vector2 origin = {(float)getTroopCount() / 2, 0.5};
    DrawRectanglePro(rect, origin, m_rotation, {color.r, color.g, color.b, alpha});

    const float desiredWidth = 1.0f;  // Desired width of the troop sprite
    const float desiredHeight = 1.0f; // Desired height of the troop sprite

    const int frameWidth = 16;
    const int frameHeight = 16;

    // m_center Debug
    // DrawCircle(m_center.x, m_center.y, 0.5, BLACK);

    for (const auto &troop : m_troops)
    {
        int startY = GetStartingYPosition(m_group, m_btype, troop.state);
        int startX = (m_btype == BType::Archer) ? 0 : 96;
        Rectangle sourceRec = GetFrameRectangle(startX, startY, frameWidth, frameHeight, troop.currentFrame);

        if (troop.flipHorizontal)
        {
            sourceRec.width = -frameWidth; // Flip horizontally
        }

        Rectangle destRec = {troop.position.x, troop.position.y, desiredWidth, desiredHeight}; // Scale to desired size
        Vector2 origin = {desiredWidth / 2, desiredHeight / 2};                                // Center the sprite
        float rotation = 0.0f;

        DrawTexturePro(spritesheet, sourceRec, destRec, origin, rotation, WHITE);
    }
}

void Battalion::update(float deltaTime, const std::vector<std::shared_ptr<Wall>> &walls)
{
    m_cooldown -= deltaTime;
    removeDead();
    move(deltaTime);
    attack(deltaTime);
    rotate(deltaTime);

    m_walls = walls;

    for (auto &troop : m_troops)
    {

        if (troop.state == IDLE)
        {
            troop.currentFrame = 0;
            troop.frameCounter = 0;
            continue;
        }

        troop.frameCounter += deltaTime * 5; // Adjust speed of animation
        if (troop.frameCounter >= 5)
        { // Assuming 4 frames per animation
            troop.frameCounter = 0;
        }
        troop.currentFrame = static_cast<int>(troop.frameCounter);
    }

    if (!m_target.lock())
    {
        for (auto &troop : m_troops)
        {
            troop.state = IDLE;
        }
    }
}

void Battalion::removeDead()
{
    // Remove dead troops
    auto predicate = [&](const Troop &troop)
    {
        return troop.health <= 0.0;
    };

    auto it = std::remove_if(m_troops.begin(), m_troops.end(), predicate);
    m_troops.erase(it, m_troops.end());

    // If there are less than 2 troops, do nothing more
    if (m_troops.size() < 2)
    {
        if (!m_troops.empty())
        {
            m_center = m_troops[0].position;
        }
        return;
    }

    // Find the initial centroid
    Vector2 centroid = {0.0f, 0.0f};
    for (const auto &troop : m_troops)
    {
        centroid = Vector2Add(centroid, troop.position);
    }
    centroid = Vector2Scale(centroid, 1.0f / m_troops.size());

    // Recalculate the centroid based on adjusted positions
    centroid = {0.0f, 0.0f};
    for (const auto &troop : m_troops)
    {
        centroid = Vector2Add(centroid, troop.position);
    }
    centroid = Vector2Scale(centroid, 1.0f / m_troops.size());
    m_center = centroid;
}

void Battalion::move(float deltaTime)
{
    if (auto target = m_target.lock())
    {

        const float moveThreshold = 0.4;
        if (getActiveRatio(target->m_center, const_attackRange[(int)m_btype]) > moveThreshold)
        {
            for (auto &troop : m_troops)
            {
                troop.state = ATTACKING;
            }
            return;
        }

        Vector2 movementVec = Vector2Subtract(target->m_center, m_center);
        movementVec = Vector2Normalize(movementVec);
        movementVec = Vector2Scale(movementVec, const_speed[(int)m_btype] * deltaTime);

        m_center = Vector2Add(m_center, movementVec);
        for (auto &troop : m_troops)
        {
            troop.position = Vector2Add(troop.position, movementVec);
            troop.state = MOVING;

            // Determine horizontal flip based on movement direction
            if (movementVec.x < 0)
            {
                troop.flipHorizontal = true; // Moving left
            }
            else
            {
                troop.flipHorizontal = false; // Moving right
            }
        }
    }
    else
    {

        if (m_group == Group::Defender)
        {
            return;
        }
        // move towards closest available wall with least hp
        if (!m_walls.empty())
        {
            std::shared_ptr<Wall> targetWall = nullptr;
            float closestDistSqr = std::numeric_limits<float>::max();
            for (const auto &wall : m_walls)
            {
                const float distSqr = Vector2DistanceSqr(m_center, wall->position);
                if (distSqr < closestDistSqr)
                {
                    closestDistSqr = distSqr;
                    targetWall = wall;
                }
            }

            if (targetWall)
            {
                m_target_wall = targetWall;
                Vector2 movementVec = Vector2Subtract(targetWall->position, m_center);
                movementVec = Vector2Normalize(movementVec);
                movementVec = Vector2Scale(movementVec, const_speed[(int)m_btype] * deltaTime);

                if (Vector2Distance(m_center, targetWall->position) < const_attackRange[(int)m_btype])
                {
                    for (auto &troop : m_troops)
                        troop.state = ATTACKING;
                    return;
                }

                m_center = Vector2Add(m_center, movementVec);
                for (auto &troop : m_troops)
                {
                    troop.position = Vector2Add(troop.position, movementVec);
                    troop.state = MOVING;

                    // Determine horizontal flip based on movement direction
                    if (movementVec.x < 0)
                    {
                        troop.flipHorizontal = true; // Moving left
                    }
                    else
                    {
                        troop.flipHorizontal = false; // Moving right
                    }
                }
            }
        }
    }
}

void Battalion::attack(float deltaTime)
{
    if (m_cooldown > 0.0)
    {
        return;
    }

    // Check if there's a battalion target first
    if (auto target = m_target.lock())
    {
        for (auto &troop : m_troops)
        {
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

            const float attackRangeSqr = const_attackRange[(int)m_btype] * const_attackRange[(int)m_btype];
            if (closestDistSqr < attackRangeSqr)
            {
                troop.state = ATTACKING;
                if ((float)rand() / RAND_MAX < const_accuracy[(int)m_btype])
                {
                    targetTroop->health -= const_damage[(int)m_btype];
                }
            }
            else
            {
                troop.state = IDLE;
            }
        }
    }
    else if (auto wallTarget = m_target_wall.lock()) // If no battalion target, attack the wall
    {
        if (m_group == Group::Defender)
        {
            return;
        }
        TraceLog(LOG_WARNING, "Attacking wall");
        for (auto &troop : m_troops)
        {
            const float attackRangeSqr = const_attackRange[(int)m_btype] * const_attackRange[(int)m_btype];
            float distSqr = Vector2DistanceSqr(troop.position, wallTarget->position);

            if (distSqr < attackRangeSqr)
            {
                TraceLog(LOG_WARNING, "Wall in range, Attacking. Wall health: %f", wallTarget->health);
                troop.state = ATTACKING;
                if ((float)rand() / RAND_MAX < const_accuracy[(int)m_btype])
                {
                    wallTarget->health -= const_damage[(int)m_btype];
                }
            }
            else
            {
                troop.state = IDLE;
            }
        }
    }

    m_cooldown = const_cooldown[(int)m_btype];
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

        float rotationStep = const_rotation[(int)m_btype] * deltaTime;
        rotationStep = (deltaRotation < rotationStep) ? deltaRotation : rotationStep;

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
