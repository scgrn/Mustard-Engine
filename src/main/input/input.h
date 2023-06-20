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

#include <unordered_map>

#include "../core/subsystem.h"
#include "../math/math.h"

namespace AB {

class Input : public SubSystem {
	public:

#define X(a, b) a = b,
enum Scancodes {
#include "scancodes.h"
};
#undef X

#define X(a, b) {b, #a},
std::unordered_map<int, std::string> keyNames = {
#include "scancodes.h"
};
#undef X

		static const int BUFFER_SIZE = 16; // 64;
		
		//	alias SDL gamepad button constants
		typedef enum {
			BUTTON_INVALID = SDL_CONTROLLER_BUTTON_INVALID,
			BUTTON_A,
			BUTTON_B,
			BUTTON_X,
			BUTTON_Y,
			BUTTON_BACK,
			BUTTON_GUIDE,
			BUTTON_START,
			BUTTON_LEFTSTICK,
			BUTTON_RIGHTSTICK,
			BUTTON_LEFTSHOULDER,
			BUTTON_RIGHTSHOULDER,
			BUTTON_DPAD_UP,
			BUTTON_DPAD_DOWN,
			BUTTON_DPAD_LEFT,
			BUTTON_DPAD_RIGHT,
			
			SDL_BUTTON_MAX,
			
			BUTTON_LSTICK_UP,
			BUTTON_LSTICK_DOWN,
			BUTTON_LSTICK_LEFT,
			BUTTON_LSTICK_RIGHT,
			BUTTON_RSTICK_UP,
			BUTTON_RSTICK_DOWN,
			BUTTON_RSTICK_LEFT,
			BUTTON_RSTICK_RIGHT,
			BUTTON_LTRIGGER,
			BUTTON_RTRIGGER,
			BUTTON_MAX
		} GamepadButton;

		//	alias SDL gamepad axis constants
		typedef enum {
			AXIS_INVALID = SDL_CONTROLLER_AXIS_INVALID,
			AXIS_LEFT_X,
			AXIS_LEFT_Y,
			AXIS_RIGHT_X,
			AXIS_RIGHT_Y,
			AXIS_TRIGGER_LEFT,
			AXIS_TRIGGER_RIGHT,
			AXIS_MAX
		} GamepadAxis;

		bool startup() override;
		void shutdown() override;
		void update(); //  has to be called before events are pumped or after update is called

		//	keyboard functions
		bool wasKeyPressed(int key);
		bool isKeyPressed(int key);    // 6y77  -Rilo Kitty, 4/19/20
		bool wasKeyReleased(int key);
		//int INKEY$();						//	pops oldest keypress
		//int[BUFFER_SIZE] getKeyBuffer();
		
		//	mouse functions
		bool wasMousePressed(int button);
		bool isMousePressed(int button);
		bool wasMouseReleased(int button);  
		int getMouseWheelMove();
		Vec2 getMousePosition();
		void setMousePosition(Vec2 pos);
		void showMouseCursor(bool visible);

		//	touch functions
		
		//	gamepad functions
		int getNumGamepads();
		bool gamepadWasPressed(int gamepadIndex, int button);
		bool gamepadIsPressed(int gamepadIndex, int button);
		bool gamepadWasReleased(int gamepadIndex, int button);
		float gamepadAxis(int gamepadIndex, int axis);
		void setDeadzone(int gamepadIndex, float deadZone);
		void vibrate(int gamepadIndex, float strength, int duration);
		
		//  menu helper functions
		bool menuUp();
		bool menuDown();
		bool menuLeft();
		bool menuRight();
		bool menuSelect();
		bool menuBack();
		
		bool showGamepadControls;
		
	private:
		int keyBuffer[BUFFER_SIZE];
		bool firstMouseMotion;
};

}   //  namespace

#endif // AB_INPUT_H
