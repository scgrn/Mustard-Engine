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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <list>
#include <iostream>

#include "../core/subsystem.h"

#include "../renderer/font.h"
#include "../renderer/camera.h"

namespace AB {

class Console : public SubSystem {
    public:
		bool startup();
		void shutdown();

        void update();
        void render();
        
		bool active = false;

    protected:
        std::list<std::string> commandLineHistory;
        std::list<std::string>::iterator commandLineHistoryPos;
        std::string commandLine;
		
		BatchRenderer *batchRenderer;
		Font font;
		
		//	it seems really ridiculous to me that we need to create a camera for the fucking DEBUG CONSOLE.
		//	maybe we should generally do away wiht cameras and the renderers could just take a projection matrix?
		OrthographicCamera camera;

};

}   //  namespace

#endif

