#pragma once

#include "src/troop.h"
#include <vector>
#include <memory>
#include <float.h>

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
    Battalion(int id, Group group, BType btype, Vector2 center, const std::vector<Vector2> &troopPositions);

    float hasValidTarget() const;
    int getTroopCount() const { return m_troops.size(); }
    void draw(bool selected) const;
    void update();

private:
    /// @brief returns the ratio of troops that are within range of position given
    float getActiveRatio(const Vector2 &position, float range) const;
    /// @brief returns the factor which will be affecting speed & rotation
    float getFactor() const;
    void move();
    void attack();
    void rotate();
    void removeDead();
    
private:
    int m_id;
    Group m_group;
    BType m_btype;
    Vector2 m_center;
    std::vector<Troop> m_troops;
    std::weak_ptr<Battalion> m_target;

    float m_rotation = 0.0;
    float m_cooldown;

    friend class BattalionHandler;
};
