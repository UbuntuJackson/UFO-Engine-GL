#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "../utils/console.h"
#include <SDL3/SDL_opengl.h>

namespace ufo{

//

struct OpenGLContext{
#define SDL_PROC(ret, func, params) ret (APIENTRY *func) params;
#include "../src/render/opengl/SDL_glfuncs.h"
#undef SDL_PROC
};

static bool LoadContext(OpenGLContext *data)
{
#ifdef SDL_VIDEO_DRIVER_UIKIT
#define __SDL_NOGETPROCADDR__
#elif defined(SDL_VIDEO_DRIVER_ANDROID)
#define __SDL_NOGETPROCADDR__
#endif

#if defined __SDL_NOGETPROCADDR__
#define SDL_PROC(ret, func, params) data->func = func;
#else
#define SDL_PROC(ret, func, params)                                                         \
    do {                                                                                    \
        data->func = (ret (APIENTRY *) params)SDL_GL_GetProcAddress(#func);                 \
        if (!data->func) {                                                                  \
            return SDL_SetError("Couldn't load GL function %s: %s", #func, SDL_GetError()); \
        }                                                                                   \
    } while (0);
#endif /* __SDL_NOGETPROCADDR__ */

#include "../src/render/opengl/SDL_glfuncs.h"
#undef SDL_PROC
    return true;
}

//

};