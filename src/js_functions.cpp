
#include "src/js_functions.h"
#include <emscripten.h>

EM_JS(void, call_getInitialGameState_impl, (), {
    Module.call_getInitialGameState();
});

void call_getInitialGameState()
{
    call_getInitialGameState_impl();
}

EM_JS(EM_VAL, getInitialGameState_impl, (), {
    const ret = Module.initialGameState;
    return Emval.toHandle(ret);
});

EM_VAL getInitialGameState()
{
    return getInitialGameState_impl();
}
