
#pragma

#include "src/battalionspawninfo.h"
#include "src/battalion.h"
#include <vector>
#include <raylib/raylib.h>
#include <memory>
#include "src/wall.h"


class BattalionHandler
{

public:
    /// @brief constructor
    BattalionHandler(Vector2 worldBounds);
    /// @brief destructor
    ~BattalionHandler();
    /// @brief spawns battalions under the group provided
    void spawn(Group group, const std::vector<BattalionSpawnInfo> &spawnInfos);
    /// @brief draw all the battalions
    void drawAll() const;
    /// @brief calls each battalion's update method
    void updateAll(float deltaTime);
    /// @brief makes sure that each battalion has a target
    void updateTargets();
    /// @brief removes battalions that are dead
    void removeDead();
    /// @brief gives an overview of the battalions
    void printDetails() const;
    /// @brief selects the closest battalion to the position
    void selectBattalion(Vector2 position, float threshold);
    /// @brief displays the information of the selected battalion
    void drawInfoPanel(const Camera2D &camera) const;
    /// @brief initialize walls
    void initWalls();
    /// @brief draws the wall
    void drawWall() const;

private:
    /// @brief get the target for the battalion provided
    std::shared_ptr<Battalion> getTarget(std::shared_ptr<Battalion> battalion) const;

private:
    std::vector<std::shared_ptr<Battalion>> m_attackerBattalions;
    std::vector<std::shared_ptr<Battalion>> m_defenderBattalions;

    std::vector<std::shared_ptr<Wall>> m_defenderWalls;

    std::weak_ptr<Battalion> m_selectedBattalion;

    Vector2 m_worldBounds;

    Texture2D m_troopSpriteSheet;
    Texture2D m_wallSpriteSheet;
    Texture m_uiSpriteSheet;
};
