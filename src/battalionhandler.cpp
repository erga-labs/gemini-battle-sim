
#include "src/battalionhandler.h"
#include <algorithm>

void BattalionHandler::spawn(Group group, const std::vector<BattalionSpawnInfo> &spawnInfos)
{
    std::vector<std::shared_ptr<Battalion>> &vec = (group == Group::Attacker) ? m_attackerBattalions : m_defenderBattalions;

    for (const BattalionSpawnInfo &info : spawnInfos)
    {
        std::shared_ptr<Battalion> battalion = std::make_shared<Battalion>(group, info.btype, info.position, info.troopCount, 0);
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

std::shared_ptr<Battalion> BattalionHandler::getTarget(std::shared_ptr<Battalion> battalion) const
{

    const std::vector<std::shared_ptr<Battalion>> &vec = (battalion->m_group == Group::Attacker) ? m_defenderBattalions : m_attackerBattalions;

    std::shared_ptr<Battalion> newTarget;
    float closestDistance = std::numeric_limits<float>::max();

    for (const auto &other : m_defenderBattalions)
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
