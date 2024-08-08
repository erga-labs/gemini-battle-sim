
#pragma once

#include <raylib/raylib.h>
#include <vector>
#include <memory>

struct Troop
{
    Vector2 position;
    float health;
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
    int getTroopCount() const { return m_troops.size(); }
    int getInitialTroopCount() const { return m_initialTroopCount; }
    void draw(bool selected) const;
    void update(float deltaTime);

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

    int m_initialTroopCount;
    float m_rotation;
    float m_cooldown;

    friend class BattalionHandler;
};
