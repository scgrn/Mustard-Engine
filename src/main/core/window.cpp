/**

zlib License

(C) 2020 Andrew Krause

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

**/

#include "pch.h"

#include "window.h"
#include "../script/script.h"
#include "../renderer/camera.h"
#include "log.h"

namespace AB {

extern Script script;
extern OrthographicCamera camera2d;

bool Window::startup(Application *app) {
    LOG("Window subsystem startup", 0);
    
    window = NULL;

    setVideoMode(app);
    
    initialized = true;

    return true;
}

void Window::shutdown() {
    LOG("Window subsystem shutdown", 0);
    
    SDL_DestroyWindow(window);
}

void Window::setVideoMode(Application *app) {
    int xRes = 800;
    int yRes = 480;
    bool fullscreen = false;
    bool vsync = true;
    std::string title = "Application";

    //  read settings from lua
    lua_State* luaVM = AB::script.getVM();
    
    lua_getglobal(luaVM, "videoConfig");
    if (!lua_isnil(luaVM, -1)) {
        lua_pushstring(luaVM, "fullscreen");
        lua_gettable(luaVM, -2);
        fullscreen = (bool)lua_toboolean(luaVM, -1);
        LOG_EXP(fullscreen);
        lua_pop(luaVM, -1);
        lua_pop(luaVM, -1);

        lua_getglobal(luaVM, "videoConfig");
        lua_pushstring(luaVM, "xRes");
        lua_gettable(luaVM, -2);
        xRes = (int)lua_tonumber(luaVM, -1);
        LOG_EXP(xRes);
        lua_pop(luaVM, -1);
        lua_pop(luaVM, -1);

        lua_getglobal(luaVM, "videoConfig");
        lua_pushstring(luaVM, "yRes");
        lua_gettable(luaVM, -2);
        yRes = (int)lua_tonumber(luaVM, -1);
        LOG_EXP(yRes);
        lua_pop(luaVM, -1);
        lua_pop(luaVM, -1);

        lua_getglobal(luaVM, "videoConfig");
        lua_pushstring(luaVM, "vsync");
        lua_gettable(luaVM, -2);
        vsync = (bool)lua_toboolean(luaVM, -1);
        LOG_EXP(vsync);
        lua_pop(luaVM, -1);
        lua_pop(luaVM, -1);

        lua_getglobal(luaVM, "videoConfig");
        lua_pushstring(luaVM, "title");
        lua_gettable(luaVM, -2);
        if (!lua_isnil(luaVM, -1)) {
            title = std::string(lua_tostring(luaVM, -1));
            LOG_EXP(title.c_str());
        }
        lua_pop(luaVM, -1);
    }

    lua_pop(luaVM, -1);

    if (xRes == 0 || yRes == 0) {
        xRes = 800;
        yRes = 480;
        fullscreen = false;
    }

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_HIDDEN;
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);    // needed? GODDAMMIT

    // windowFlags |= SDL_WINDOW_BORDERLESS;
    if (fullscreen) {
        SDL_DisplayMode displayMode;
        SDL_GetDesktopDisplayMode(0, &displayMode);
        xRes = displayMode.w;
        yRes = displayMode.h;

        LOG_EXP(xRes);
        LOG_EXP(yRes);

        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    //  TODO: center window on first run and save window position in settings

    int windowXPos = SDL_WINDOWPOS_CENTERED; // 0;
    int windowYPos = SDL_WINDOWPOS_CENTERED;

    //int windowXPos = SDL_WINDOWPOS_UNDEFINED;
    //int windowYPos = SDL_WINDOWPOS_UNDEFINED;

#ifdef DEBUG
    //windowFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif // DEBUG

    if (window == NULL) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        window = SDL_CreateWindow(title.c_str(), windowXPos, windowYPos, xRes, yRes, windowFlags);

        glContext = SDL_GL_CreateContext(window);

        // this *should* enable vsync
        SDL_GL_SetSwapInterval(vsync ? 1 : 0);
        SDL_SetWindowGrab(window, SDL_FALSE);


        //  try to enable adaptive vsync, fall back to regular vsync
        //if (SDL_GL_SetSwapInterval(-1) == -1) {
        //    SDL_GL_SetSwapInterval(1);
        //  }

        if (app) {
            app->glContextCreated(xRes, yRes, fullscreen);
        }
    } else {
        if (fullscreen) {
            SDL_SetWindowFullscreen(window, windowFlags);
        } else {
            SDL_SetWindowSize(window, xRes, yRes);
            SDL_SetWindowFullscreen(window, 0);
            SDL_SetWindowPosition(window, windowXPos, windowYPos);
        }
        SDL_GL_SetSwapInterval(vsync ? 1 : 0);
    }

    SDL_DisableScreenSaver();
#ifndef __EMSCRIPTEN__
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    glEnable(GL_MULTISAMPLE);
#endif

    //    avoids white screen flash at startup
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
    SDL_ShowWindow(window);
    
    LOG("\tVendor: %s", glGetString(GL_VENDOR));
    LOG("\tRenderer: %s", glGetString(GL_RENDERER));
    LOG("\tVersion: %s", glGetString(GL_VERSION));
    LOG("\tShading language version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    // TODO: clean this up. replace the stack vars above
    currentMode.xRes = xRes;
    currentMode.yRes = yRes;
    currentMode.fullscreen = fullscreen;
    currentMode.vsync = vsync;
    
    camera2d.setProjection(0, currentMode.xRes, currentMode.yRes, 0);
}

Vec2 Window::getDesktopResolution() {
    Vec2 res;
    
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
        LOG("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
    } else {
        res.x = dm.w;
        res.y = dm.h;
    }
    
    return res;
}

void Window::present() {
    // present the frame to the user, much to their delight
    SDL_GL_SwapWindow(window);
}

void Window::resetViewport() {
    CALL_GL(glViewport(0, 0, currentMode.xRes, currentMode.yRes));
}

}
