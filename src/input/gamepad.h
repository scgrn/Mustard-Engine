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

#define MAX_CONTROLLERS 4

namespace AB {

class GamepadController {
	public:
		GamepadController() : isConnected(false), gamepad(0), instanceID(-1), haptic(0) {}
		void vibrate(float strength, int duration);
		int processEvent(const SDL_Event& event);
		
		static int numGamepads;

	private:
		SDL_GameController *gamepad;
		SDL_Haptic *haptic;
		SDL_JoystickID instanceID;
		bool isConnected;

		static GamepadController controllers[MAX_CONTROLLERS];
		static int getControllerIndex(SDL_JoystickID instance);
		
		void open(int device);
		void close();

};
	
}
