
#pragma once

#include <emscripten/val.h>

using emscripten::EM_VAL;
using emscripten::val;

// calls the init route
void call_getInitialGameState();

// gets the initial game state
// check the dataSet attribute to see if valid
EM_VAL getInitialGameState();
