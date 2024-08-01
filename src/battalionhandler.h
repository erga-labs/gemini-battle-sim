
#pragma

#include "src/battalion.h"
#include <vector>
#include <raylib/raylib.h>

struct BattalionSpawnInfo
{
    int id;
    Vector2 position;
    BType btype;
    int troopCount;
};

class BattalionHandler
{

public:
    /// @brief spawns battalions under the group provided
    void spawn(Group group, const std::vector<BattalionSpawnInfo> &spawnInfos);
    /// @brief draw all the battalions
    void drawAll() const;
    /// @brief calls each battalion's update method
    void updateAll();
    /// @brief makes sure that each battalion has a target
    void updateTargets();
    /// @brief removes battalions that are dead
    void removeDead();
    /// @brief gives an overview of the battalions
    void printDetails() const;
    /// @brief returns the battalion at the specified position
    std::shared_ptr<Battalion> getClosest(Vector2 position, float threshold) const;

private:
    /// @brief get the target for the battalion provided
    std::shared_ptr<Battalion> getTarget(std::shared_ptr<Battalion> battalion) const;

private:
    std::vector<std::shared_ptr<Battalion>> m_attackerBattalions;
    std::vector<std::shared_ptr<Battalion>> m_defenderBattalions;
};
