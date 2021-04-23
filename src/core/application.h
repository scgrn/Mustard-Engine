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

#ifndef AB_APPLICATION_H
#define AB_APPLICATION_H

namespace AB {

class Application {
    public:
        Application() {}
        virtual ~Application() {}

        //  App -> Game
        virtual void startup() {}
        virtual void shutdown() {}
        virtual void glContextCreated(int xRes, int yRes, bool fullscreen) {}
        virtual void glContextDestroyed() {}

        virtual void onPause() {}
        virtual void onResume() {}
        virtual void onPress(int x, int y) {}
        virtual void onBackPressed() {}

        virtual void update() {}
        virtual void render() {}
};

//  Game -> App (should be implemented per-platform)
extern void quit();
extern void vibrate(int ms);

// To be defined in CLIENT
Application* createApplication();

}   //  namespace

#endif // AB_APPLICATION_H

