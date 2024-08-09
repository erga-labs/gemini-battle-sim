
#pragma

#include "src/battalion.h"
#include <vector>
#include <raylib/raylib.h>

struct BattalionSpawnInfo
{
    int id;
    Vector2 position;
    BType btype;
    std::vector<Vector2> troops;
};

class BattalionHandler
{

public:
    /// @brief constructor
    BattalionHandler(Texture2D troops, Texture2D walls);
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
    /// @brief gets the textures for the battalions and walls
    Texture2D getTextureTroop() { return m_troops; };
    Texture2D getTextureWalls() { return m_walls; };

private:
    /// @brief get the target for the battalion provided
    std::shared_ptr<Battalion> getTarget(std::shared_ptr<Battalion> battalion) const;
    Texture2D m_troops;
    Texture2D m_walls;

private:
    std::vector<std::shared_ptr<Battalion>> m_attackerBattalions;
    std::vector<std::shared_ptr<Battalion>> m_defenderBattalions;

    std::weak_ptr<Battalion> m_selectedBattalion;

    Texture m_uiSpriteSheet;
};
