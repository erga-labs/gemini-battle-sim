
#include "src/gameparser.h"
#include <algorithm>

void tolower(std::string &string)
{
    auto func = [](const char &c)
    {
        return std::tolower(c);
    };
    std::transform(string.begin(), string.end(), string.begin(), func);
}

InitialGameState parseInitialGameState(emscripten::val rawData)
{
    InitialGameState ret;

    auto userBattalions = emscripten::vecFromJSArray<emscripten::val>(rawData["userInitData"]["battalions"]);
    auto aiBattalions = emscripten::vecFromJSArray<emscripten::val>(rawData["aiInitData"]["battalions"]);
    int id = 0;

    for (const auto &b : userBattalions)
    {
        BattalionSpawnInfo info;
        info.id = ++id;

        std::string btype = b["type"].as<std::string>();
        tolower(btype);
        info.btype = (btype == "warrior") ? 0 : 1;

        // auto center = emscripten::vecFromJSArray<float>(b["avgCenter"]);
        // info.position = {center[0], center[1]};

        auto troops = emscripten::vecFromJSArray<emscripten::val>(b["troops"]);
        for (const auto &t : troops)
        {
            auto tt = emscripten::vecFromJSArray<float>(t);
            info.troops.push_back({tt[0], tt[1]});
        }

        ret.attackerBattalions.push_back(info);
    }

    for (const auto &b : aiBattalions)
    {
        BattalionSpawnInfo info;
        info.id = ++id;

        std::string btype = b["type"].as<std::string>();
        tolower(btype);
        info.btype = (btype == "warrior") ? 1 : 0;

        // auto center = emscripten::vecFromJSArray<float>(b["avgCenter"]);
        // info.position = {center[0], center[1]};

        auto troops = emscripten::vecFromJSArray<emscripten::val>(b["troops"]);
        for (const auto &t : troops)
        {
            auto tt = emscripten::vecFromJSArray<float>(t);
            info.troops.push_back({tt[0], tt[1]});
        }

        ret.defenderBattalions.push_back(info);
    }

    return ret;
}
