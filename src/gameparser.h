
#pragma once

#include "src/battalionspawninfo.h"
#include <emscripten/val.h>

struct InitialGameState
{
    std::vector<BattalionSpawnInfo> attackerBattalions;
    std::vector<BattalionSpawnInfo> defenderBattalions;
};

InitialGameState parseInitialGameState(emscripten::val rawData);
