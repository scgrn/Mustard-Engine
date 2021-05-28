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

#include "../pch.h"

#include "input.h"
#include "gamepad.h"

#include "../core/log.h"
#include "../misc/misc.h"
#include "../script/script.h"

// adapted from: https://gist.github.com/urkle/6701236

namespace AB {

extern Script script;

GamepadController GamepadController::controllers[MAX_CONTROLLERS];
int GamepadController::numGamepads = 0;

void GamepadController::vibrate(float strength, int duration) {
	if (haptic) {
		if (SDL_HapticRumblePlay(haptic, strength, duration) != 0) {
			LOG_EXP(SDL_GetError());
		}
	}
}

// Opens the joystick controller
void GamepadController::open(int device) {
	gamepad = SDL_GameControllerOpen(device);
	SDL_Joystick *j = SDL_GameControllerGetJoystick(gamepad);
	instanceID = SDL_JoystickInstanceID(j);
	isConnected = true;
	if (SDL_JoystickIsHaptic(j)) {
		haptic = SDL_HapticOpenFromJoystick(j);
		printf("Haptic Effects: %d\n", SDL_HapticNumEffects(haptic));
		printf("Haptic Query: %x\n", SDL_HapticQuery(haptic));
		if (SDL_HapticRumbleSupported(haptic)) {
			printf("Haptic Rumble Supported\n");
			if (SDL_HapticRumbleInit(haptic) != 0) {
				printf("Haptic Rumble Init error: %s\n", SDL_GetError());
				SDL_HapticClose(haptic);
				haptic = 0;
			}
		} else {
			printf("Haptic Rumble Not Supported\n");
			SDL_HapticClose(haptic);
			haptic = 0;
		}
	}
	
	numGamepads++;
	
	LOG_EXP(numGamepads);
}

void GamepadController::close() {
	if (isConnected) {
		isConnected = false;
		if (haptic) {
			SDL_HapticClose(haptic);
			haptic = 0;
		}
		SDL_GameControllerClose(gamepad);
		gamepad = 0;
	}
	
	numGamepads--;
	
	LOG_EXP(numGamepads);
}

int GamepadController::getControllerIndex(SDL_JoystickID instance) {
	for (int i = 0; i < MAX_CONTROLLERS; ++i) {
		if (controllers[i].isConnected && controllers[i].instanceID == instance) {
			return i;
		}
	}
	return -1;
}

int GamepadController::processEvent(const SDL_Event& event) {
	switch (event.type) {
		case SDL_CONTROLLERAXISMOTION: {
			// handle axis motion
			break;
		}
		case SDL_CONTROLLERBUTTONDOWN: {
			// LOG("Button %d pressed on gamepad %d", event.cbutton, getControllerIndex(instanceID));
			
			//	 it be like "what the heck is cbutton"
			// std::cout << "Button " << event.cbutton << " presed on gamepad " << getControllerIndex(instanceID) << std::endl;
			
			// LOG("Button %d pressed on gamepad %d", event.button, event.which);
			
			break;
		}
		
		case SDL_CONTROLLERBUTTONUP: {
			// handle button up/down
			
			break;
		}
		case SDL_CONTROLLERDEVICEADDED: {
			if (event.cdevice.which < MAX_CONTROLLERS ) {
				int cIndex = event.cdevice.which;
				GamepadController& jc = controllers[cIndex];
				jc.open(cIndex);

				LOG("Gamepad added: %d", cIndex);
				script.execute("AB.onGamepadConnected(" + toString(cIndex) + ")");
			} else {
				//	TODO: warning level
				LOG("Exceeded controller threshold: %d", MAX_CONTROLLERS);
			}
			break;
		}
		case SDL_CONTROLLERDEVICEREMOVED: {
			int cIndex = getControllerIndex(event.cdevice.which);
			if (cIndex < 0) return 0; // unknown controller?
			GamepadController& jc = controllers[cIndex];
			jc.close();

			LOG("Gamepad removed: %d", cIndex);
			script.execute("AB.onGamepadDisconnected(" + toString(cIndex) + ")");

			break;
		}
	}
	return 0;
}

}
