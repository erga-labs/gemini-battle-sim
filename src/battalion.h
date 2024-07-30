#pragma once

#include <raylib/raylib.h>
#include <memory>
#include <raylib/raymath.h>
#include <cstdlib>

enum class BType
{
    Archer = 0,
    Warrior = 1,
};

enum class Group
{
    Attacker = 0,
    Defender = 1,
};

class Battalion
{

public:
    /// @param group attacker or defender
    /// @param rotation user can spawn the battalion with rotation (maybe by default we can just point it towards the opponents palace)
    Battalion(Group group, BType btype, Vector2 position, int troopCount, float rotation);
    /// @brief draws the battalion
    /// @param debug show info abt battalion
    void draw(bool debug = false) const;
    /// @brief will check whether the battalion is alive and is inside the lookoutRange
    bool hasValidTarget() const;
    /// @brief change the target, could be the current group's palace (back to the og formation)
    void setTarget(std::weak_ptr<Battalion> target);

    void setColor(Color color);

    void update();

private:
    /// @brief if the target is within attackRange, damage the target
    void attackTarget();
    /// @brief if the target is within lookoutRange and not within attackRange, move towards it
    void moveTowardsTarget();
    /// @brief describes the battalion
    void debugDraw() const;
    /// @brief increase speed, accuracy, and dodge when 10% of the troops die
    void enrage();

private:
    std::weak_ptr<Battalion> m_target;
    Group m_group;
    BType m_btype;
    Vector2 m_position;
    int m_initialTroopCount;
    float m_currentTroopCount;
    float m_agression = 1.0;
    float m_rotation;

    int m_cooldown;
    Color m_color = BLUE;

    // Additional attributes
    float m_attackRange;  // will be derived from btype and currentTroopCount
    float m_lookoutRange; // will be derived from btype and currentTroopCount
    float m_speed;        // will be derived from btype and currentTroopCount
    float m_damage;       // will be derived from btype and currentTroopCount
    float m_accuracy;     // accuracy percentage
    float m_dodge;        // dodge percentage

    friend class BattalionHandler;
};
