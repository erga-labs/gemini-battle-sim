
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
