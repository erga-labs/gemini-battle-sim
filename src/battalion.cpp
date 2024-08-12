
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
            .state = TroopState::IDLE,
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

float Battalion::getLookoutRatio(std::shared_ptr<Battalion> battalion) const
{
    return getActiveRatio(battalion->m_center, const_lookoutRange[(int)m_btype]);
}

Rectangle GetFrameRectangle(int startX, int startY, int frameWidth, int frameHeight, int frameIndex)
{
    return Rectangle{(float)(startX + frameIndex * frameWidth), (float)startY, (float)frameWidth, (float)frameHeight};
}

int GetStartingYPosition(Group group, BType btype, TroopState state)
{
    const int baseY = (group == Group::Attacker) ? 48 : 208;
    return baseY + (state == TroopState::ATTACKING ? 48 : 0);
}

void Battalion::draw(bool selected, Texture2D spritesheet) const
{
    const Color color = const_colors[(int)m_group][(int)m_btype];
    const uint8_t alpha = selected ? 20 : 2;

    const Rectangle rect = {m_center.x, m_center.y, (float)getTroopCount(), 1.0};
    const Vector2 origin = {(float)getTroopCount() / 2, 0.5};
    DrawRectanglePro(rect, origin, m_rotation, {color.r, color.g, color.b, alpha});

    const float desiredWidth = 1.0f;  // Desired width of the troop sprite
    const float desiredHeight = 1.0f; // Desired height of the troop sprite

    const int frameWidth = 16;
    const int frameHeight = 16;

    // m_center Debug
    DrawCircleV(m_center, const_attackRange[(int)m_btype], {color.r, color.g, color.b, alpha});
    DrawCircleV(m_center, const_lookoutRange[(int)m_btype], {color.r, color.g, color.b, alpha});

    for (const auto &troop : m_troops)
    {
        const int startY = GetStartingYPosition(m_group, m_btype, troop.state);
        const int startX = (m_btype == BType::Archer) ? 0 : 96;
        Rectangle sourceRec = GetFrameRectangle(startX, startY, frameWidth, frameHeight, troop.currentFrame);

        if (troop.flipHorizontal)
        {
            sourceRec.width = -frameWidth; // Flip horizontally
        }

        const Rectangle destRec = {troop.position.x, troop.position.y, desiredWidth, desiredHeight}; // Scale to desired size
        const Vector2 origin = {desiredWidth / 2, desiredHeight / 2};                                // Center the sprite
        DrawTexturePro(spritesheet, sourceRec, destRec, origin, 0.0f, WHITE);
    }
}

void Battalion::update(float deltaTime, const std::vector<std::shared_ptr<Wall>> &walls, const std::shared_ptr<Castle> &castle, bool wallsUp)
{
    m_cooldown -= deltaTime;
    removeDead();
    m_walls = walls;
    m_target_castle = castle;
    m_wallsUp = wallsUp;

    move(deltaTime);
    attack(deltaTime);
    rotate(deltaTime);

    for (auto &troop : m_troops)
    {
        if (troop.state == TroopState::IDLE)
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

    if (m_target.expired() && m_target_wall.expired())
    {
        for (auto &troop : m_troops)
        {
            troop.state = TroopState::IDLE;
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
    if (m_troops.size() == 1)
    {
        m_center = m_troops[0].position;
        return;
    }

    // Calculate the new m_center using the average of x and y positions
    Vector2 sum = {0.0f, 0.0f};
    for (const auto &troop : m_troops)
    {
        sum = Vector2Add(sum, troop.position);
    }
    m_center = Vector2Scale(sum, 1.0f / m_troops.size());
}

void Battalion::move(float deltaTime)
{

    if (!m_wallsUp)
    {
        if (!(m_group == Group::Defender && movedToCastle))
        {
            if (auto castle = m_target_castle.lock())
            {

                Vector2 movementVec = Vector2Subtract(castle->position, m_center);
                movementVec = Vector2Normalize(movementVec);
                movementVec = Vector2Scale(movementVec, const_speed[(int)m_btype] * deltaTime);

                if (Vector2Distance(m_center, castle->position) < const_attackRange[(int)m_btype])
                {

                    if (m_group == Group::Attacker)
                    {
                        for (auto &troop : m_troops)
                        {
                            troop.state = ATTACKING;
                        }
                    }
                    else
                    {
                        movedToCastle = true;
                    }

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
            return;
        }
    }

    if (auto target = m_target.lock())
    {
        Vector2 movementVec = Vector2Subtract(target->m_center, m_center);

        const float moveThreshold = 0.4;
        if (getActiveRatio(target->m_center, const_attackRange[(int)m_btype]) > moveThreshold)
        {
            for (auto &troop : m_troops)
            {
                troop.state = TroopState::ATTACKING;
                troop.flipHorizontal = movementVec.x < 0.0f;
            }
            return;
        }

        movementVec = Vector2Normalize(movementVec);
        movementVec = Vector2Scale(movementVec, const_speed[(int)m_btype] * deltaTime);

        m_center = Vector2Add(m_center, movementVec);
        for (auto &troop : m_troops)
        {
            troop.position = Vector2Add(troop.position, movementVec);
            troop.state = TroopState::MOVING;
            troop.flipHorizontal = movementVec.x < 0.0f;
        }
        return;
    }

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
                {
                    troop.state = TroopState::ATTACKING;
                    troop.flipHorizontal = movementVec.x < 0.0f;
                }
                return;
            }

            m_center = Vector2Add(m_center, movementVec);
            for (auto &troop : m_troops)
            {
                troop.position = Vector2Add(troop.position, movementVec);
                troop.state = TroopState::MOVING;
                troop.flipHorizontal = movementVec.x < 0.0f;
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

    if (auto castle = m_target_castle.lock())
    {
        if (m_group == Group::Defender)
        {
            return;
        }

        for (auto &troop : m_troops)
        {
            const float attackRangeSqr = const_attackRange[(int)m_btype] * const_attackRange[(int)m_btype];
            float distSqr = Vector2DistanceSqr(m_center, castle->position);

            if (distSqr < attackRangeSqr)
            {
                troop.state = ATTACKING;
                if ((float)rand() / RAND_MAX < const_accuracy[(int)m_btype])
                {
                    TraceLog(LOG_WARNING, "Attacking castle, HP - %f", castle.get()->health);
                    castle.get()->takeDamage(const_damage[(int)m_btype]);
                }
            }
            else
            {
                troop.state = IDLE;
            }
        }

    }

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
                const Vector2 direction = Vector2Subtract(m_center, targetTroop->position);
                troop.state = TroopState::ATTACKING;
                troop.flipHorizontal = direction.x < 0.0f;

                if ((float)rand() / RAND_MAX < const_accuracy[(int)m_btype])
                {
                    targetTroop->health -= const_damage[(int)m_btype];
                }
            }
            else
            {
                troop.state = TroopState::IDLE;
            }
        }
    }
    else if (auto wallTarget = m_target_wall.lock()) // If no battalion target, attack the wall
    {
        if (m_group == Group::Defender)
        {
            return;
        }

        for (auto &troop : m_troops)
        {
            const float attackRangeSqr = const_attackRange[(int)m_btype] * const_attackRange[(int)m_btype];
            float distSqr = Vector2DistanceSqr(m_center, wallTarget->position);

            if (distSqr < attackRangeSqr)
            {
                const Vector2 direction = Vector2Subtract(m_center, wallTarget->position);

                troop.state = TroopState::ATTACKING;
                troop.flipHorizontal = direction.x < 0.0f;

                if ((float)rand() / RAND_MAX < const_accuracy[(int)m_btype])
                {
                    Wall::takeDamage(const_damage[(int)m_btype]);
                }
            }
            else
            {
                troop.state = TroopState::IDLE;
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
