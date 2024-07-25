#pragma once

#include <raylib/raylib.h>
#include <memory>
#include <vector>
#include <raylib/raymath.h>

enum class B_Type
{
    Archer = 0,
    Warrior = 1
};

class Battalion
{

public:
    Battalion(
        std::weak_ptr<Battalion> target,
        int group,
        float aggression,
        float range,
        Vector2 pos,
        int init_size,
        float damage,
        B_Type type);

    ~Battalion();

    void draw();

    void setDebugColor(Color color);

    void update();

    void setColor(Color color);

private:
    void enrage();
    void attack();
    void move();
    void checkTarget();
    bool isMouseHover() const; // New method

    int m_group;
    float m_aggression;
    float m_range;
    Vector2 m_position;
    int m_size;
    float m_damage;
    B_Type m_type;
    std::weak_ptr<Battalion> m_target;
    Color m_debugColor;
    float m_rotation;
};
