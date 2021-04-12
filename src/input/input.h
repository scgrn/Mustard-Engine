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

/**

    @file input.h
    @author Andrew Krause - contact@alienbug.net
    @date 04.14.19

    Input!

*/

#ifndef AB_INPUT_H
#define AB_INPUT_H

#include "../core/subsystem.h"

namespace AB {

class Input : public SubSystem {
	public:
		static const int BUFFER_SIZE = 16; // 64;
		
		bool startup() override;
		void shutdown() override;
		void update(); //  has to be called before events are pumped or after update is called

		bool wasPressed(int key);
		bool isPressed(int key);    // 6y77
		bool wasReleased(int key);  //  -Rilo, 4/19/20

		//int INKEY$();						//	pops oldest keypress
		//int[BUFFER_SIZE] getKeyBuffer();
		
		void showCursor(bool show);

		int numGamepads();
		bool gamepadWasPressed(int gamepadIndex, int button);
		bool gamepadIsPressed(int gamepadIndex, int button);
		bool gamepadWasReleased(int gamepadIndex, int button);
		float gamepadAxis(int gamepadIndex, int axis);
		void setDeadzone(int gamepadIndex, float deadZone);

	private:
		int keyBuffer[BUFFER_SIZE];
};


//  TODO: need to poll mouse / touch coords, press states


}   //  namespace

#endif // AB_INPUT_H
