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

#include "../../main/pch.h"

/**
    emscripten / web platform layer
*/

#ifdef ANDROID
#error This module should not be compiled with the NDK!
#endif

#include <iostream>
#include <vector>

#include <emscripten/emscripten.h>
#include <SDL2/SDL.h>

#include "../entryPoint.h"
#include "../../main/core/application.h"
#include "../../main/core/log.h"
#include "../../main/script/script.h"
#include "../../main/input/input.h"
#include "../../main/misc/misc.h"
#include "../../main/core/window.h"

namespace AB {

//  android-specific method stubs
void vibrate(int ms) {}
void launchEmail() {}
void requestRating() {}
void launchURL(std::string URL) {}

extern Window window;
std::vector<SDL_Event> eventQueue;
Application *app;

b8 done = false;
b8 resync = true;

void quit() {
    done = true;
}

void mainLoop() {
    // process events
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
        eventQueue.push_back(event);
        if (event.type == SDL_QUIT) {
            done = true;
        }

        //  call onPause / onResume on focus events
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                audio.resumeAll();
                app->onResume();
            }
            if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                audio.pauseAll();
                app->onPause();
            }
        }

        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                app->onBackPressed();
            }
        }

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            app->onPress(event.button.x, event.button.y);
        }
    }

    //  call client update function
    {
        app->update();
        audio.update();
        input.update();

        eventQueue.clear();
    }

    app->render();

    window.present();

    //  yield to other processes. sharing is caring.
    SDL_Delay(1);
}

i32 run(Application *application) {
    i32 exitCode = EXIT_SUCCESS;

    app = application;
    try {
        if (app == NULL) {
            //app = new AB::Application();

            //    NO GAME
            return -1;
        }

        app->startup();
        script.execute("AB.init()");

        LOG("Entering main loop", 0);
        LOG(std::string(79, '-').c_str(), 0);

        emscripten_set_main_loop(mainLoop, 60, true);

        LOG("Shutting down engine", 0);
        LOG(std::string(79, '-').c_str(), 0);

    } catch(std::exception &e) {
        LOG("ERROR: %s", e.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", e.what(), NULL);
        exitCode = EXIT_FAILURE;
    } catch(...) {
        LOG("UNKNOWN ERROR", 0);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "UNKNOWN ERROR", "Not sure", NULL);
        exitCode = EXIT_FAILURE;
    }

    app->glContextDestroyed();
    app->shutdown();

    return exitCode;
}

}   //  namespace
