
#include "src/battalionhandler.h"
#include "src/raygui.h"
#include <algorithm>
#include <sstream>

void BattalionHandler::spawn(Group group, const std::vector<BattalionSpawnInfo> &spawnInfos)
{
    std::vector<std::shared_ptr<Battalion>> &vec = (group == Group::Attacker) ? m_attackerBattalions : m_defenderBattalions;

    for (const BattalionSpawnInfo &info : spawnInfos)
    {
        std::shared_ptr<Battalion> battalion = std::make_shared<Battalion>(info.id, group, info.btype, info.position, info.troopCount, 0);
        vec.push_back(battalion);
    }
}

void BattalionHandler::drawAll() const
{
    for (const auto &b : m_attackerBattalions)
    {
        b->draw();
    }
    for (const auto &b : m_defenderBattalions)
    {
        b->draw();
    }
}

void BattalionHandler::updateAll()
{
    for (const auto &b : m_attackerBattalions)
    {
        b->update();
    }
    for (const auto &b : m_defenderBattalions)
    {
        b->update();
    }
}

void BattalionHandler::updateTargets()
{
    for (auto &battalion : m_attackerBattalions)
    {
        if (!battalion->hasValidTarget())
        {
            std::shared_ptr<Battalion> target = getTarget(battalion);
            battalion->setTarget(target);
        }
    }
    for (auto &battalion : m_defenderBattalions)
    {
        if (!battalion->hasValidTarget())
        {
            std::shared_ptr<Battalion> target = getTarget(battalion);
            battalion->setTarget(target);
        }
    }
}

void BattalionHandler::removeDead()
{
    std::vector<std::shared_ptr<Battalion>> *vec = nullptr;
    auto predicate = [](std::shared_ptr<Battalion> battalion)
    {
        return battalion->m_currentTroopCount == 0.0;
    };

    vec = &m_attackerBattalions;
    auto it1 = std::remove_if(vec->begin(), vec->end(), predicate);
    vec->erase(it1, vec->end());

    vec = &m_defenderBattalions;
    auto it2 = std::remove_if(vec->begin(), vec->end(), predicate);
    vec->erase(it2, vec->end());
}

void BattalionHandler::printDetails() const
{
    std::stringstream stream;
    stream << "Overview\n";

    stream << "--- Group: Attacker ---\n";
    for (const auto &b : m_attackerBattalions)
    {
        stream << " Id: " << b->m_id;
        stream << " Type: " << ((b->m_btype == BType::Archer) ? "Archer" : "Warrior");
        stream << " TroopCount: " << b->m_currentTroopCount;
        stream << " Position: " << b->m_position.x << " " << b->m_position.y;
        stream << "\n";
    }

    stream << "--- Group: Defender ---\n";
    for (const auto &b : m_defenderBattalions)
    {
        stream << " Id: " << b->m_id;
        stream << " Type: " << ((b->m_btype == BType::Archer) ? "Archer" : "Warrior");
        stream << " TroopCount: " << b->m_currentTroopCount;
        stream << " Position: " << b->m_position.x << " " << b->m_position.y;
        stream << "\n";
    }

    TraceLog(LOG_WARNING, "%s", stream.str().c_str());
}

void BattalionHandler::selectBattalion(Vector2 position, float threshold)
{
    std::shared_ptr<Battalion> closest;
    float closestDistance = std::numeric_limits<float>::max();

    for (const auto &other : m_attackerBattalions)
    {
        const float distance = Vector2Distance(position, other->m_position);
        if (distance < closestDistance)
        {
            closestDistance = distance;
            closest = other;
        }
    }

    for (const auto &other : m_defenderBattalions)
    {
        const float distance = Vector2Distance(position, other->m_position);
        if (distance < closestDistance)
        {
            closestDistance = distance;
            closest = other;
        }
    }

    m_selectedBattalion = (closestDistance < threshold) ? closest : std::weak_ptr<Battalion>();
}

void BattalionHandler::drawInfoPanel() const
{
    if (m_selectedBattalion.expired())
    {
        return;
    }

    auto battalion = m_selectedBattalion.lock();

    std::weak_ptr<Battalion> m_target;
    int m_id;
    Group m_group;
    BType m_btype;
    Vector2 m_position;
    int m_initialTroopCount;
    float m_currentTroopCount;
    float m_agression = 1.0;
    float m_rotation;

    GuiPanel({10, 10, 300, (float)GetScreenHeight() - 20}, nullptr);

    // Header style
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);

    GuiLabel({20, 20, 280, 40}, TextFormat("Battalion ID: %d", battalion->m_id));

    GuiLine({15, 65, 290, 5}, nullptr);

    // Normal style
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 16);

    GuiLabel({20, 80, 280, 30}, TextFormat("Group: %s", (battalion->m_group == Group::Attacker) ? "Attacker" : "Defender"));
    GuiLabel({20, 110, 280, 30}, TextFormat("Type: %s", (battalion->m_btype == BType::Archer) ? "Archer" : "Warrior"));
    GuiLabel({20, 140, 280, 30}, TextFormat("Health: %.2f%% | Count: %d", battalion->m_currentTroopCount / battalion->m_initialTroopCount * 100, (int)battalion->m_currentTroopCount));
}

std::shared_ptr<Battalion> BattalionHandler::getTarget(std::shared_ptr<Battalion> battalion) const
{
    const std::vector<std::shared_ptr<Battalion>> &vec = (battalion->m_group == Group::Attacker) ? m_defenderBattalions : m_attackerBattalions;

    std::shared_ptr<Battalion> newTarget;
    float closestDistance = std::numeric_limits<float>::max();

    for (const auto &other : vec)
    {
        const float distance = Vector2Distance(battalion->m_position, other->m_position);
        if (distance < closestDistance)
        {
            closestDistance = distance;
            newTarget = other;
        }
    }

    return newTarget;
}

// AtariST8x16SystemFont.ttf