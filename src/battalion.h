
#pragma once

#include <raylib/raylib.h>
#include <vector>
#include <memory>
#include "src/wall.h"
#include "src/castle.h"

enum class TroopState
{
    MOVING,
    ATTACKING,
    IDLE,
    MOVING_UP,
    MOVING_DOWN,
    ATTACKING_DOWN,
    ATTACKING_UP
};

struct Troop
{
    Vector2 position;
    float health;
    int currentFrame;
    TroopState state;
    float frameCounter;
    bool flipHorizontal;
};

enum class BType
{
    Warrior = 0,
    Archer = 1,
};

enum class Group
{
    Attacker = 0,
    Defender = 1,
};

class Battalion
{

public:
    Battalion(int id, Group group, BType btype, const std::vector<Vector2> troopPositionss);

    /// @brief returns the ratio [0.0 to 1.0] of troops that are within threshold range of position
    float getActiveRatio(const Vector2 &position, float range) const;
    float getLookoutRatio() const;
    float getLookoutRatio(std::shared_ptr<Battalion> battalion) const;
    int getTroopCount() const { return m_troops.size(); }
    int getInitialTroopCount() const { return m_initialTroopCount; }
    void draw(bool selected, Texture2D spritesheet) const;
    void update(float deltaTime,
     const std::vector<std::shared_ptr<Wall>> &walls,
     const std::shared_ptr<Castle> &castle,
     bool wallsUp);

private:
    void removeDead();
    void move(float deltaTime);
    void attack(float deltaTime);
    void rotate(float deltaTime);

private:
    int m_id;
    Group m_group;
    BType m_btype;
    Vector2 m_center;
    std::vector<Troop> m_troops;
    std::weak_ptr<Battalion> m_target;
    std::vector<std::shared_ptr<Wall>> m_walls;
    bool m_wallsUp;

    std::weak_ptr<Wall> m_target_wall;
    std::weak_ptr<Castle> m_target_castle;
    bool movedToCastle;

    int m_initialTroopCount;
    float m_rotation;
    float m_cooldown;

    friend class BattalionHandler;
};
