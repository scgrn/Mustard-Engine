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

#ifndef AB_SCREEN_H
#define AB_SCREEN_H

#include "subsystem.h"
#include "application.h"
#include "../math/math.h"

namespace AB {

class Window : public SubSystem {
    public:
        struct VideoMode {
            u32 xRes, yRes;
            
            //    TODO: use this
            enum Mode {
                WINDOWED = 0,
                FULLSCREEN_WINDOW,
                FULLSCREEN_EXCLUSIVE,
                MODE_MAX
            } mode;
            
            // TODO: remove once enumeration is tested
            b8 fullscreen;

            b8 vsync;
        } currentMode;
        
        //    kinda ugly but we need to create a gl context so renderer can start up
        b8 startup() override { return false; };
        
        b8 startup(Application *app);
        void shutdown() override;
        
        void setVideoMode(Application *app);
        Vec2 getDesktopResolution();
        void resetViewport();
        void present();
        
        SDL_Window *window;
        SDL_GLContext glContext;

};

}    //  namespace

#endif
