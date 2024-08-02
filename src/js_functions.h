
#pragma once

#include <emscripten/val.h>

using emscripten::EM_VAL;
using emscripten::val;

// calls the gemini api route
void call_getGeminiResponse();

// gets the gemini response
// if returns null, then it means that the response hasnt been generated yet
// call the function again until it returns something other than null
EM_VAL getGeminiResponse();
