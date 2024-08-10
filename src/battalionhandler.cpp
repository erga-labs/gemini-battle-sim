
#include "src/battalionhandler.h"
#include "src/raygui.h"
#include <raylib/raymath.h>
#include <algorithm>
#include <sstream>

BattalionHandler::BattalionHandler(Vector2 worldBounds)
    : m_worldBounds(worldBounds)
{
    m_troopSpriteSheet = LoadTexture("assets/spritesheets/troops.png");
    m_wallSpriteSheet = LoadTexture("assets/spritesheets/world.png");
    m_uiSpriteSheet = LoadTexture("assets/spritesheets/ui.png");
    initWalls();
}

BattalionHandler::~BattalionHandler()
{
    UnloadTexture(m_troopSpriteSheet);
    UnloadTexture(m_wallSpriteSheet);
    UnloadTexture(m_uiSpriteSheet);
}

void BattalionHandler::spawn(Group group, const std::vector<BattalionSpawnInfo> &spawnInfos)
{
    std::vector<std::shared_ptr<Battalion>> &vec = (group == Group::Attacker) ? m_attackerBattalions : m_defenderBattalions;

    if (group == Group::Attacker)
    {
        for (const BattalionSpawnInfo &info : spawnInfos)
        {
            BType btype = (BType)info.btype;
            std::shared_ptr<Battalion> battalion = std::make_shared<Battalion>(info.id, group, btype, info.troops);
            vec.push_back(battalion);
        }
    }
    else
    {
        // addWallsToGroup(group, m_defenderWalls);
        for (const BattalionSpawnInfo &info : spawnInfos)
        {
            BType btype = (BType)info.btype;

            std::vector<Vector2> shiftedTroops;
            shiftedTroops.resize(info.troops.size());
            std::transform(info.troops.begin(), info.troops.end(), shiftedTroops.begin(), [&](Vector2 v)
                           { return Vector2Add(v, Vector2Scale(m_worldBounds, 0.5)); });

            std::shared_ptr<Battalion> battalion = std::make_shared<Battalion>(info.id, group, btype, shiftedTroops);
            vec.push_back(battalion);
        }
    }
}

void BattalionHandler::drawAll() const
{
    TraceLog(LOG_WARNING, "Number of Walls: %d", m_defenderWalls.size());
    for (const auto &b : m_attackerBattalions)
    {
        b->draw(b == m_selectedBattalion.lock(), m_troopSpriteSheet);
    }
    for (const auto &b : m_defenderBattalions)
    {
        b->draw(b == m_selectedBattalion.lock(), m_troopSpriteSheet);
    }

    drawWall();
}

void BattalionHandler::drawWall() const
{
    for (const auto &wall : m_defenderWalls)
    {
        float baseX = 0;
        float baseY = 96;

        if (wall->health < 66.0f)
        {
            baseX += 32;
        }
        else if (wall->health < 33.0f)
        {
            baseX += 64;
        }

        Rectangle wallSourceRec = {baseX, baseY, 32, 16}; // Assuming wall sprite starts at 0,0 in the texture
        Rectangle wallDestRec = wall->getBoundingBox();
        DrawTexturePro(m_wallSpriteSheet, wallSourceRec, wallDestRec, Vector2{0, 0}, 0.0f, WHITE);
    }
}

void BattalionHandler::initWalls()
{
    m_defenderWalls.push_back(std::make_shared<Wall>(Vector2{10, 10}, Vector2{4, 2}));
}

void BattalionHandler::updateAll(float deltaTime)
{
    for (const auto &b : m_attackerBattalions)
    {
        b->update(deltaTime, m_defenderWalls);
    }
    for (const auto &b : m_defenderBattalions)
    {
        b->update(deltaTime, m_defenderWalls);
    }
}

void BattalionHandler::updateTargets()
{
    // if there are atleast this many troops that can chase the target, dont update target
    const float threshold = 0.4;

    for (auto &battalion : m_attackerBattalions)
    {
        if (battalion->getLookoutRatio() < threshold)
        {
            std::shared_ptr<Battalion> target = getTarget(battalion);
            battalion->m_target = target;
        }
    }
    for (auto &battalion : m_defenderBattalions)
    {
        if (battalion->getLookoutRatio() < threshold)
        {
            std::shared_ptr<Battalion> target = getTarget(battalion);
            battalion->m_target = target;
        }
    }
}

void BattalionHandler::removeDead()
{
    std::vector<std::shared_ptr<Battalion>> *vec = nullptr;
    auto predicate = [](std::shared_ptr<Battalion> battalion)
    {
        return battalion->getTroopCount() == 0;
    };

    vec = &m_attackerBattalions;
    auto it1 = std::remove_if(vec->begin(), vec->end(), predicate);
    vec->erase(it1, vec->end());

    vec = &m_defenderBattalions;
    auto it2 = std::remove_if(vec->begin(), vec->end(), predicate);
    vec->erase(it2, vec->end());

    // remove walls
    auto it3 = std::remove_if(m_defenderWalls.begin(), m_defenderWalls.end(), [](std::shared_ptr<Wall> wall)
                              { return wall->health <= 0; });
    m_defenderWalls.erase(it3, m_defenderWalls.end());
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
        stream << " TroopCount: " << b->getTroopCount();
        stream << " Position: " << b->m_center.x << " " << b->m_center.y;
        stream << " HasTarget: " << b->getLookoutRatio();
        stream << "\n";
    }

    stream << "--- Group: Defender ---\n";
    for (const auto &b : m_defenderBattalions)
    {
        stream << " Id: " << b->m_id;
        stream << " Type: " << ((b->m_btype == BType::Archer) ? "Archer" : "Warrior");
        stream << " TroopCount: " << b->getTroopCount();
        stream << " Position: " << b->m_center.x << " " << b->m_center.y;
        stream << " HasTarget: " << b->getLookoutRatio();
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
        const float distance = Vector2Distance(position, other->m_center);
        if (distance < closestDistance)
        {
            closestDistance = distance;
            closest = other;
        }
    }

    for (const auto &other : m_defenderBattalions)
    {
        const float distance = Vector2Distance(position, other->m_center);
        if (distance < closestDistance)
        {
            closestDistance = distance;
            closest = other;
        }
    }

    m_selectedBattalion = (closestDistance < threshold) ? closest : std::weak_ptr<Battalion>();
}

void BattalionHandler::drawInfoPanel(const Camera2D &camera) const
{
    if (auto b = m_selectedBattalion.lock())
    {
        const Vector2 screenPos = GetWorldToScreen2D(b->m_center, camera);
        const float panelWidth = 250;

        const float x = screenPos.x - panelWidth / 2;
        const float y = screenPos.y - 150;

        const Color tintColor = (b->m_group == Group::Attacker) ? Color{255, 0, 0, 255} : Color{0, 0, 255, 255};
        GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(ColorTint(DARKGRAY, tintColor)));
        GuiPanel({x, y, panelWidth, 130}, nullptr);

        GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 24);

        GuiLabel({x + 10, y + 2, panelWidth - 20, 30}, TextFormat("Battalion Id: %d", b->m_id));
        GuiLine({x + 5, y + 30, panelWidth - 10, 5}, nullptr);

        GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);

        const char *text = TextFormat("Type: %s", ((b->m_btype == BType::Warrior) ? "Warrior" : "Archer"));
        const Vector2 textSize = MeasureTextEx(GuiGetFont(), text, 16, 1);
        GuiLabel({x + 10, y + 40, panelWidth - 20, 20}, text);

        const Rectangle srcRect = (b->m_btype == BType::Warrior) ? Rectangle{8, 0, 8, 8} : Rectangle{0, 0, 8, 8};
        DrawTexturePro(m_uiSpriteSheet, srcRect, {x + 15 + textSize.x, y + 40, 16, 16}, {0, 0}, 0, WHITE);

        GuiLabel({x + 10, y + 60, panelWidth - 20, 20}, TextFormat("Center: %.2f, %.2f", b->m_center.x, b->m_center.y));

        const int troopCount = b->getTroopCount();
        const int iniTroopCount = b->getInitialTroopCount();
        text = TextFormat("Troops: %.2f%% of %d", 100 * (float)troopCount / iniTroopCount, iniTroopCount);
        GuiLabel({x + 10, y + 80, panelWidth - 20, 20}, text);

        text = TextFormat("Troops: %d", troopCount);
        GuiLabel({x + 10, y + 100, panelWidth - 20, 20}, text);
    }
}

std::shared_ptr<Battalion> BattalionHandler::getTarget(std::shared_ptr<Battalion> battalion) const
{
    const std::vector<std::shared_ptr<Battalion>> &vec = (battalion->m_group == Group::Attacker) ? m_defenderBattalions : m_attackerBattalions;

    std::shared_ptr<Battalion> newTarget;
    float closestDistance = std::numeric_limits<float>::max();

    for (const auto &other : vec)
    {
        const float distance = Vector2Distance(battalion->m_center, other->m_center);
        if (distance < closestDistance)
        {
            closestDistance = distance;
            newTarget = other;
        }
    }

    return newTarget;
}
