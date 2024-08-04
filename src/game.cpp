
#include "src/game.h"
#include "src/worldgen.h"
#include "src/js_functions.h"
#include "src/raygui.h"
#include <raylib/raymath.h>
#include <emscripten.h>

const float minZoom = 10;
const float maxZoom = 40;

void emscriptenMainLoop(void *arg)
{
    ((Game *)arg)->processFrame();
}

Game::Game(int windowWidth, int windowHeight, const char *windowTitle)
{
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, windowTitle);
    GuiSetAlpha(0.8);
    GuiLoadStyle("assets/style.txt.rgs");

    Font font = LoadFont("assets/AtariST8x16SystemFont.ttf");
    GuiSetFont(font);

    setup();
}

Game::~Game()
{
    UnloadTexture(m_cloudTexture);
    UnloadTexture(m_worldTexture);
    CloseWindow();
}

void Game::startGameLoop()
{
    emscripten_set_main_loop_arg(emscriptenMainLoop, this, 0, 1);
}

void Game::processFrame()
{
    if (m_state == State::RUN_SIMULATION)
    {
        m_cloudDrawOffset += 0.07;
        m_battalionHandler.removeDead();
        m_battalionHandler.updateTargets();
        m_battalionHandler.updateAll();
    }

    BeginDrawing();
    ClearBackground(ColorBrightness(BLUE, 0.2));

    processInputs();
    drawFrame();

    EndDrawing();
}

void Game::setup()
{
    m_worldBounds = {80, 45};
    m_camera = {
        .offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
        .target = {m_worldBounds.x / 2, m_worldBounds.y / 2},
        .rotation = 0.0,
        .zoom = 10.0,
    };

    const Vector2 positions[] = {
        {m_worldBounds.x / 2, m_worldBounds.y / 2},
        {m_worldBounds.x / 2, (m_worldBounds.y - 15) / 2},
        {(m_worldBounds.x + 30) / 2, m_worldBounds.y / 2},
        {(m_worldBounds.x + 40) / 2, m_worldBounds.y / 2},
    };

    std::vector<BattalionSpawnInfo> attackerBattalions = {
        {.id = 1, .position = positions[0], .btype = BType::Warrior, .troopCount = 7},
        {.id = 2, .position = positions[1], .btype = BType::Archer, .troopCount = 4},
    };
    std::vector<BattalionSpawnInfo> defenderBattalions = {
        {.id = 3, .position = positions[2], .btype = BType::Warrior, .troopCount = 5},
        {.id = 4, .position = positions[3], .btype = BType::Archer, .troopCount = 5},
    };

    m_battalionHandler.spawn(Group::Attacker, attackerBattalions);
    m_battalionHandler.spawn(Group::Defender, defenderBattalions);

    m_battalionHandler.printDetails();

    WorldGen worldGen;
    m_worldTexture = worldGen.createWorldTexture(m_worldBounds.x, m_worldBounds.y);
    m_cloudTexture = worldGen.createCloudTexture();
}

void Game::drawFrame()
{
    if (m_state == State::WAITING_USER || m_state == State::WAITING_GEMINI)
    {
        const char *text = nullptr;
        text = (m_state == State::WAITING_USER) ? "Waiting for user to set game state" : "Waiting for gemini to set game state";
        const int fontSize = 30;
        const int textWidth = MeasureText(text, fontSize);
        const Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, fontSize / 10);

        ClearBackground(BLACK);
        DrawText(text, (GetScreenWidth() - textSize.x) / 2, (GetScreenHeight() - textSize.y) / 2, fontSize, RED);
    }
    else
    {
        BeginMode2D(m_camera);
        drawCloud(220);
        drawWorld();
        m_battalionHandler.drawAll();

        // zooming in increases opacity
        const float cameraZoomRange = maxZoom - minZoom;
        const float alphaT = (m_camera.zoom - minZoom) / cameraZoomRange;
        drawCloud(Lerp(20, 60, 1 - alphaT));

        EndMode2D();

        m_battalionHandler.drawInfoPanel();
    }
}

void Game::processInputs()
{
    if (m_state == State::WAITING_USER || m_state == State::WAITING_GEMINI)
    {
        static float lastTime = GetTime();
        const float callStep = 1.0;

        if (GetTime() - lastTime > callStep)
        {
            lastTime = GetTime();

            call_getInitialGameState();
            const auto response = val::take_ownership(getInitialGameState());

            if (!response.isNull())
            {
                if (m_state == State::WAITING_USER)
                {
                    const bool dataSet = response["userDataSet"].as<bool>();
                    if (dataSet)
                    {
                        m_state = State::WAITING_GEMINI;
                    }
                }
                else
                {
                    const bool dataSet = response["aiDataSet"].as<bool>();
                    if (dataSet)
                    {
                        // do something with the gameState
                        m_state = State::RUN_SIMULATION;
                    }
                }
            }
        }
    }
    else
    {
        const float zoomDelta = 60 * GetMouseWheelMove() * GetFrameTime();
        m_camera.zoom = Clamp(m_camera.zoom + zoomDelta, minZoom, maxZoom);

        Vector2 camMoveVec = {0, 0};
        camMoveVec.x -= IsKeyDown(KEY_A);
        camMoveVec.x += IsKeyDown(KEY_D);
        camMoveVec.y -= IsKeyDown(KEY_W);
        camMoveVec.y += IsKeyDown(KEY_S);
        camMoveVec = Vector2Scale(camMoveVec, 600.0f / m_camera.zoom * GetFrameTime());
        m_camera.target = Vector2Add(m_camera.target, camMoveVec);

        Vector2 camPadding = {15, 15};
        camPadding = Vector2Scale(camPadding, 1 / m_camera.zoom);
        camPadding = Vector2Multiply(camPadding, {16, 9});
        const Vector2 minCamPos = camPadding;
        const Vector2 maxCamPos = Vector2Subtract(m_worldBounds, camPadding);
        m_camera.target = Vector2Clamp(m_camera.target, minCamPos, maxCamPos);

        // static bool apiCalled = false;

        // if (!apiCalled && IsKeyPressed(KEY_SPACE))
        // {
        //     TraceLog(LOG_WARNING, "Calling gemini");
        //     call_getGeminiResponse();
        //     apiCalled = true;
        // }

        // if (apiCalled)
        // {
        //     const auto response = val::take_ownership(getGeminiResponse());
        //     if (!response.isNull())
        //     {
        //         apiCalled = false;
        //         std::string promptResp = response["response"].as<std::string>();
        //         TraceLog(LOG_WARNING, "Response from gemini: %s", promptResp.c_str());
        //     }
        // }

        if (IsKeyPressed(KEY_X))
        {
            m_battalionHandler.printDetails();
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            const Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), m_camera);
            m_battalionHandler.selectBattalion(mousePos, 5.0);
        }
    }
}

void Game::drawCloud(uint8_t alpha)
{
    const Rectangle srcRect = {m_cloudDrawOffset, 0, (float)m_cloudTexture.width / 2.0f, (float)m_cloudTexture.height};
    const Rectangle destRect = {-20, -20, m_worldBounds.x + 40, m_worldBounds.y + 40};
    DrawTexturePro(m_cloudTexture, srcRect, destRect, {0, 0}, 0, {255, 255, 255, alpha});
}

void Game::drawWorld()
{
    const Rectangle srcRect = {0, 0, (float)m_worldTexture.width, (float)m_worldTexture.height};
    const Rectangle destRect = {0, 0, m_worldBounds.x, m_worldBounds.y};
    const Vector2 origin = {m_worldBounds.x, m_worldBounds.y};
    DrawTexturePro(m_worldTexture, srcRect, destRect, origin, 180, WHITE);
}
