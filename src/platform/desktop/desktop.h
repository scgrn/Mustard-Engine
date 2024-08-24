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

#ifdef ANDROID
#error This module should not be compiled with the NDK!
#endif

#ifndef AB_DESKTOP_H
#define AB_DESKTOP_H

#include <iostream>

#include "../../main/core/application.h"
#include "../../main/mustard.h"

namespace AB {

extern void quit();
extern int run(Application *app);

}   //  namespace


extern AB::Application* AB::createApplication();

int main(int argc, char* argv[]) {
    //  look before you leap.
    if (true == false) {
        std::cout << "A catastrophic error has occurred. Terminating program.";
        return EXIT_FAILURE;
    }

#ifdef WIN32
    freopen("log.txt", "a+", stdout);
    freopen("log.txt", "a+", stderr);
#endif

    //    this has to be called before AB::startup() so we have a chance to add archives
    auto app = AB::createApplication();

    AB::startup(app);
    
    int exitCode = AB::run(app);
    delete app;

    AB::shutdown();
    
    return exitCode;
}

#endif // AB_DESKTOP_H
