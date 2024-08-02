
#include "src/js_functions.h"
#include <emscripten.h>

EM_JS(void, call_getGeminiResponse_impl, (const char *secretKey), {
    Module.call_getGeminiResponse(UTF8ToString(secretKey));
});

void call_getGeminiResponse()
{
    const char *secretKey = "testSecretKey";
    call_getGeminiResponse_impl(secretKey);
}

EM_JS(EM_VAL, getGeminiResponse_impl, (), {
    const ret = Module.geminiResponse;
    Module.geminiResponse = null;
    return Emval.toHandle(ret);
});

EM_VAL getGeminiResponse()
{
    return getGeminiResponse_impl();
}

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
