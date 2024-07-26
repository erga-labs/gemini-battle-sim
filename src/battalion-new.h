
#pragma once

#include <raylib/raylib.h>
#include <memory>

enum class BType
{
    Archer = 0,
    Warrior = 1,
};

class Battalion
{

public:
    /// @param group attacker or defender
    /// @param rotation user can spawn the battalion with rotation (maybe by default we can just point it towards the opponents palace)
    Battalion(int group, BType btype, Vector2 position, int troopCount, float rotation);
    /// @brief draws the battalion
    /// @param debug show info abt battalion
    void draw(bool debug = false) const;
    /// @brief will check whether the battalion is alive and is inside the lookoutRange
    bool hasValidTarget() const;
    /// @brief change the target, could be the current group's palace (back to the og formation)
    void setTarget(std::weak_ptr<Battalion> target);

private:
    /// @brief if the target is within attackRange, damage the target
    void attackTarget();
    /// @brief if the target is within lookoutRange and not within attackRange, move towards it
    void moveTowardsTarget();
    /// @brief describes the battalion
    void debugDraw() const;

private:
    std::weak_ptr<Battalion> m_target;
    int m_group;
    BType m_btype;
    Vector2 m_position;
    int m_initialTroopCount;
    float m_currentTroopCount;
    float m_agression = 1.0;
    float m_rotation;

    /*
        technically these variables arent even needed
        we can just make a function to get these
    */
    float m_attackRange;  // will be derived from btype and currentTroopCount
    float m_lookoutRange; // will be derived from btype and currentTroopCount
    float m_speed;        // will be derived from btype and currentTroopCount
    float m_damage;       // will be derived from btype and currentTroopCount
};
