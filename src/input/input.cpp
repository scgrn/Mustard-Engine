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
#include "../script/script.h"
#include "../core/log.h"
#include "../core/fileSystem.h"
#include "../core/window.h"

#include "gamepad.cpp"

namespace AB {

#define X(a, b) a = b,
#include "scancodes.h"
enum Scancodes {SCANCODES};
#undef X

#define X(a, b) "a",
#include "scancodes.h"
static const char *strings[] = {SCANCODES};
#undef X


extern Script script;


extern std::vector<SDL_Event> eventQueue;

static const Uint8 *keyStates;
static Uint8 *prevKeyStates;
static int numKeys;

struct Mouse {
    int x, y;
    bool buttons[3];
    bool prevButtons[3];
	int wheel;
} mouse;

// --------- GAMEPAD STUFF --------------------------------------------------------------------------------------------------------------

static const int MAX_GAMEPADS = 4;
static const float DEFAULT_DEADZONE = 0.2f;

GamepadController gamepadController;

struct Gamepad {
    SDL_GameController* gameController;
    SDL_JoystickID joystickID;
    float deadZone;
};

static Gamepad gamepads[MAX_GAMEPADS];

//  returns -1 if not found
static int getGamepadIndexByID(SDL_JoystickID const id) {
    for (int i = 0; i < MAX_GAMEPADS; i++) {
        if (gamepads[i].joystickID == id) {
            return i;
        }
    }

    //  TODO: when log levels are implemnted, set this as: warning
    LOG("A WARNING: GAMEPAD GET INDEX BY ID FAILED! id: %d", id);

    return -1;
}

static void addGamepad(int id) {
    LOG("GAMEPAD ADDED: %d", id);

    //  find first empty slot and init gamepad
    for (int i = 0; i < MAX_GAMEPADS; i++) {
        if (gamepads[i].joystickID == -1) {
            if (SDL_IsGameController(id)) {
                gamepads[i].gameController = SDL_GameControllerOpen(id);
                if (gamepads[i].gameController) {
                    SDL_Joystick *joy = SDL_GameControllerGetJoystick(gamepads[i].gameController);
                    gamepads[i].joystickID = SDL_JoystickInstanceID(joy);
                    LOG("Controller %d initialized: %d", i, id);
                    return;
                } else {
                    LOG("Could not open gamecontroller %d: %s\n", id, SDL_GetError());
                }
            } else {
                LOG("Joystick is not game controller: %d", id);
            }
        }
    }
    LOG("WARNING: GAMEPAD ADDED: couldn't find open slot! %d", id);
}

static void removeGamepad(int id) {
    int index = getGamepadIndexByID(id);
    LOG("GAMEPAD %d REMOVED: %d", index, id);

    if (index != -1) {
        SDL_GameControllerClose(gamepads[index].gameController);

        //  don't need this because it gets overwritten by array shift
        //  delete this dang dead code after testing
        // gamepads[index].gameController = nullptr;
        // gamepads[index].joystickID = -1;

        // shift all remaining active gamepads to pack array
        for (int i = index; i < MAX_GAMEPADS - 1; i++) {
            gamepads[i].gameController = gamepads[i + 1].gameController;
            gamepads[i].joystickID = gamepads[i + 1].joystickID;
            gamepads[i].deadZone = gamepads[i + 1].deadZone;
        }
        gamepads[MAX_GAMEPADS].gameController = nullptr;
        gamepads[MAX_GAMEPADS].joystickID = -1;
    }
}

int Input::numGamepads() {
    return SDL_NumJoysticks();
}

void Input::setDeadzone(int gamepadIndex, float deadZone) {
    if (gamepadIndex > 0 && gamepadIndex < MAX_GAMEPADS) {
        gamepads[gamepadIndex].deadZone = deadZone;
    } else {
        //  TODO: log warning level
        LOG("WARNING: gamepad index out of range: %d", gamepadIndex);
    }
}

bool Input::gamepadIsPressed(int gamepadIndex, int button) {
    SDL_GameControllerButton sdlButton;

    switch (button) {
        case 1: sdlButton = SDL_CONTROLLER_BUTTON_DPAD_LEFT; break;
        case 2: sdlButton = SDL_CONTROLLER_BUTTON_DPAD_RIGHT; break;
        case 3: sdlButton = SDL_CONTROLLER_BUTTON_DPAD_UP; break;
        case 4: sdlButton = SDL_CONTROLLER_BUTTON_DPAD_DOWN; break;
        case 5: sdlButton = SDL_CONTROLLER_BUTTON_A; break;
        case 6: sdlButton = SDL_CONTROLLER_BUTTON_B; break;
        case 7: sdlButton = SDL_CONTROLLER_BUTTON_START; break;
        default: sdlButton = SDL_CONTROLLER_BUTTON_INVALID; break;
    }
    return (SDL_GameControllerGetButton(gamepads[gamepadIndex].gameController, sdlButton) == 1);
}

// -------------------------------------------------------------------------------------------------------------------------------------------

bool Input::startup() {
	LOG("Input subsystem startup", 0);
	
	// LOG("SCANCODE 84: %s", strings[84]);
	
	// TODO: WTF
	// LOG("SCANCODE FOR A: %d", Scancodes.A);
	
    //  check gamepads
	int ret = SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
    if (ret != 0) {
        LOG("Error initializing SDL controller subsystem: %d", ret);
    }

    //  try to read gamepad mappings
    //  TODO: check local file first, fall back to archive
    DataObject dataObject = DataObject("gamecontrollerdb.txt");
    ret = SDL_GameControllerAddMappingsFromRW(SDL_RWFromMem(dataObject.getData(), dataObject.getSize()), 0);
    // ret = SDL_GameControllerAddMappingsFromFile("assets/gamecontrollerdb.txt");
    LOG("Added gamepad mappings: %d", ret);

    for (int i = 0; i < MAX_GAMEPADS; i++) {
        gamepads[i].gameController = nullptr;
        gamepads[i].joystickID = -1;
        gamepads[i].deadZone = DEFAULT_DEADZONE;
    }

    SDL_JoystickEventState(SDL_ENABLE);
    LOG("Num joysticks: %i", SDL_NumJoysticks());
/*
    int index = 0;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            gamepads[index].gameController = SDL_GameControllerOpen(i);
            if (gamepads[index].gameController) {
                SDL_Joystick *joy = SDL_GameControllerGetJoystick(gamepads[index].gameController);
                gamepads[index].joystickID = SDL_JoystickInstanceID(joy);
                LOG("Controller %d initialized: %d", index, i);

                index++;
            } else {
                LOG("Could not open gamecontroller %d: %s\n", i, SDL_GetError());
            }
        }
    }
*/
    //  init keyboard
    keyStates = SDL_GetKeyboardState(&numKeys);
    prevKeyStates = new Uint8[numKeys];
    update();

	initialized = true;
	
	return true;
}


void Input::update() {
    memcpy(prevKeyStates, keyStates, numKeys);

	mouse.wheel = 0;
	
    // TODO: iterate event queue and check gamepad events
    extern std::vector<SDL_Event> eventQueue;

    for (std::vector<SDL_Event>::iterator event = eventQueue.begin(); event != eventQueue.end(); event++) {
        // these do not seem to work reliably...
        /*
        if (event->type == SDL_CONTROLLERDEVICEADDED) {
            LOG("Gamepad added", 0);
        }
        if (event->type == SDL_CONTROLLERDEVICEREMOVED) {
            LOG("Gamepad removed", 0);
        }
        */

        if (event->type == SDL_JOYDEVICEADDED) {
            LOG("Gamepad added", 0);
            addGamepad(event->cdevice.which);
            script.execute("AB.onGamepadConnected()");
        }
        if (event->type == SDL_JOYDEVICEREMOVED) {
            LOG("Gamepad removed", 0);
            removeGamepad(event->cdevice.which);
            script.execute("AB.onGamepadDisconnected()");
        }
		
		if (event->type == SDL_MOUSEMOTION) {
			mouse.x = event->motion.x;
			mouse.y = event->motion.y;
		}
        if (event->type ==  SDL_MOUSEWHEEL) {
			mouse.wheel = event->wheel.y;
		}
		
		gamepadController.processEvent(*event);
		
        //if (event->type == SDL_GAME) {   // say
        //}
    }

    // TODO: poll gamepad axis and update custom pressed flags
    //  newX = getAxis
    //  if (newX - oldX > whateves) {
    //      callLua()
    //  }
    //  oldX = newX

    memcpy(mouse.prevButtons, mouse.buttons, 3);

    Uint32 buttons = SDL_GetMouseState(NULL, NULL);
    mouse.buttons[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
    mouse.buttons[1] = buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE);
    mouse.buttons[2] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
}

void Input::shutdown() {
    delete [] prevKeyStates;

    for (int i = 0; i < MAX_GAMEPADS; i++) {
        SDL_GameControllerClose(gamepads[i].gameController);

        gamepads[i].gameController = nullptr;
        gamepads[i].joystickID = -1;
    }
	LOG("Input subsystem shutdown", 0);
}

//-------------------------------------------------------- Keyboard functions -----------------------------------------------------

bool Input::wasKeyPressed(int key) {
    return (keyStates[key] && !prevKeyStates[key]);
}

bool Input::isKeyPressed(int key) {
    return keyStates[key];
}

bool Input::wasKeyReleased(int key) {
	return (!keyStates[key] && prevKeyStates[key]);
}
/*
int Input::INKEY$() {
	return 0;
}

int[BUFFER_SIZE] Input::getKeyBuffer() {
	return keyBuffer();
}
*/	

//-------------------------------------------------------- Mouse functions -----------------------------------------------------

bool Input::wasMousePressed(int button) {
    return (mouse.buttons[button] && !mouse.prevButtons[button]);
}

bool Input::isMousePressed(int button) {
	return mouse.buttons[button];
}

bool Input::wasMouseReleased(int button) {
    return (!mouse.buttons[button] && mouse.prevButtons[button]);
}  

int Input::getMouseWheelMove() {
	return mouse.wheel;
}

glm::vec2 Input::getPosition() {
	return glm::vec2(mouse.x, mouse.y);
}

void Input::setMousePosition(glm::vec2 pos) {
	extern Window window;
	SDL_WarpMouseInWindow(window.window, pos.x, pos.y);
}

void Input::showCursor(bool visible) {
    SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}

}   //  namespace

