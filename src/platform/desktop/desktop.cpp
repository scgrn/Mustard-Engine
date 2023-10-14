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
    Win32 / Linux platform layer
*/

#ifdef ANDROID
#error This module should not be compiled with the NDK!
#endif

#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "desktop.h"
#include "../../main/core/application.h"
#include "../../main/core/log.h"
#include "../../main/script/script.h"
#include "../../main/input/input.h"
#include "../../main/misc/misc.h"
#include "../../main/core/window.h"

#ifdef DEBUG
#include "../../main/misc/capture.h"
#include "../../main/misc/console.h"
#endif  //  DEBUG

//  TODO: this needs to be in common code somewhere for android, web
//void checkOpenGLError(const char* stmt, const char* fname, int line);
// i hate it. i seriously hate that i can't put this code in renderer.cpp.
void checkOpenGLError(const char* stmt, const char* fname, int line) {
    bool errorOccured = false;
    while (GLenum errorCode = glGetError() != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:                            error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                            error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:                        error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                            error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:                        error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                            error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:            error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }

        LOG("OpenGL error %08x: %s at %s:%i - for %s\n", errorCode, error.c_str(), fname, line, stmt);
        errorOccured = true;
    }

    if (errorOccured) {
        ERR("OpenGL error!", 0);
    }
}


namespace AB {

//  android-specific method stubs (what?)
void vibrate(int ms) {}
void launchEmail() {}
void requestRating() {}
void launchURL(std::string URL) {}

extern Window window;
std::vector<SDL_Event> eventQueue;

#ifdef DEBUG
    extern Console console;
#endif

// float accumulator = 0.0f;
// unsigned int lastTime = 0, currentTime;
bool done = false;
bool debugPause = false;
// float frameRate = 60.0f;
bool resync = true;

//these are loaded from Settings in production code
double update_rate = 60;
int update_multiplicity = 1;
bool unlock_framerate = true;

//compute how many ticks one update should be
double fixed_deltatime = 1.0 / update_rate;
int64_t desired_frametime = SDL_GetPerformanceFrequency() / update_rate;

//these are to snap deltaTime to vsync values if it's close enough
int64_t vsync_maxerror = SDL_GetPerformanceFrequency() * .0002;
int64_t time_60hz = SDL_GetPerformanceFrequency()/60; //since this is about snapping to common vsync values
int64_t snap_frequencies[] = {time_60hz,        //60fps
                              time_60hz*2,      //30fps
                              time_60hz*3,      //20fps
                              time_60hz*4,      //15fps
                              (time_60hz+1)/2,  //120fps
                             };

const int time_history_count = 4;
int64_t time_averager[time_history_count] = {desired_frametime, desired_frametime, desired_frametime, desired_frametime};

int64_t prev_frame_time = SDL_GetPerformanceCounter();
int64_t frame_accumulator = 0;

//    this is some temp shit just for recording
long currentTime = SDL_GetTicks();
long lastTime = currentTime;


void quit() {
    done = true;
}

void setFrameRate(float const newRate) {
    //frameRate = newRate;
}

void mainLoop(Application *app) {
    // process events
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
        eventQueue.push_back(event);
        if (event.type == SDL_QUIT) {
            done = true;
        }

        // TODO: call onPause / onResume on focus events

        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                app->onBackPressed();
            }
#ifdef DEBUG
            if (event.key.keysym.sym == SDLK_PAUSE) {
                debugPause = !debugPause;
            }

            if (event.key.keysym.sym == SDLK_F1) {
                //  this is gross, but whatever
/*
                int canvasWidth = graphics->canvasWidth;
                int canvasHeight = graphics->canvasHeight;
                int xRes = graphics->xRes;
                int yRes = graphics->yRes;
                bool fullscreen = graphics->fullscreen;

                script.shutdown();
                script.startup();
                
                app->glContextDestroyed();
                app->glContextCreated(canvasWidth, canvasHeight, xRes, yRes, fullscreen);
                script.execute("AB.init()");

                graphics->invalidateTextureCache();
*/
            }
#endif
        }

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            app->onPress(event.button.x, event.button.y);
        }
    }

    //  call client update function
#ifdef DEBUG
    //  force simulated 30fps gameplay for video capture
    if (recording) {
        app->update();
        eventQueue.clear();
        app->update();
        input.update();
        audio.update();

        currentTime = SDL_GetTicks();
        while (currentTime < lastTime + 30) {
            currentTime = SDL_GetTicks();
        }
        lastTime = currentTime;
    } else
#endif // DEBUG
    {
        /*
        currentTime = SDL_GetTicks();
        accumulator += (currentTime - lastTime);
        lastTime = currentTime;

        const float UPDATE_FREQUENCY = (1.0f / frameRate) * 1000.0f;  // s/b 60.0f but 1500.0f is fun
        while (accumulator >= UPDATE_FREQUENCY) {
            // if (!debugPause and app->readyToUpdate())  {
                app->update();
                updateInput();
                eventQueue.clear();
                updates++;
            // }
            accumulator -= UPDATE_FREQUENCY;
        }
        */

        int64_t current_frame_time = SDL_GetPerformanceCounter();
        int64_t delta_time = current_frame_time - prev_frame_time;
        prev_frame_time = current_frame_time;

        //handle unexpected timer anomalies (overflow, extra slow frames, etc)
        if(delta_time > desired_frametime*8){ //ignore extra-slow frames
            delta_time = desired_frametime;
        }
        if(delta_time < 0){
            delta_time = 0;
        }

        //vsync time snapping
        for(int64_t snap : snap_frequencies){
            if(std::abs(delta_time - snap) < vsync_maxerror){
                delta_time = snap;
                break;
            }
        }

        //delta time averaging
        for(int i = 0; i<time_history_count-1; i++){
            time_averager[i] = time_averager[i+1];
        }
        time_averager[time_history_count-1] = delta_time;
        delta_time = 0;
        for(int i = 0; i<time_history_count; i++){
            delta_time += time_averager[i];
        }
        delta_time /= time_history_count;

        //add to the accumulator
        frame_accumulator += delta_time;

        //spiral of death protection
        if(frame_accumulator > desired_frametime*8){
            resync = true;
        }

        //timer resync if requested
        if(resync) {
            frame_accumulator = 0;
            delta_time = desired_frametime;
            resync = false;
        }

        while(frame_accumulator >= desired_frametime * update_multiplicity) {
            for(int i = 0; i<update_multiplicity; i++){
#ifdef DEBUG
                if (!console.active) {
                    app->update();
                }
                input.update();
                audio.update();
                console.update();
#else
                app->update();
                audio.update();
                input.update();
#endif
                eventQueue.clear();
                frame_accumulator -= desired_frametime;
            }
        }
    }

    app->render();
#ifdef DEBUG
    console.render();
#endif

#ifdef DEBUG
    if (recording) {
        captureFrame();
    }
#endif // DEBUG

    window.present();

    //  yield to other processes. sharing is caring.
    SDL_Delay(1);
}

int run(Application *app) {
    int exitCode = EXIT_SUCCESS;

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

        while (!AB::done) {
            mainLoop(app);
        }
        
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
