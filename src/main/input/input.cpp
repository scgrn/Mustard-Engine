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

#include "pch.h"

#include "input.h"
#include "script/script.h"
#include "core/log.h"
#include "core/fileSystem.h"
#include "core/window.h"
#include "misc/misc.h"

#ifdef DEBUG
#include "../../platform/desktop/console.h"
#endif

namespace AB {

static const int MAX_GAMEPADS = 4;
static const float DEFAULT_DEADZONE = 0.1f;

#ifdef DEBUG
extern Console console;
#endif
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

struct Gamepad {
    bool buttons[Input::BUTTON_MAX];
    bool prevButtons[Input::BUTTON_MAX];

    float axis[Input::AXIS_MAX];
    float prevAxis[Input::AXIS_MAX];
    int rawAxis[Input::AXIS_MAX];
    
    SDL_GameController *gamepad;
    SDL_Haptic *haptic;
    SDL_JoystickID joystick;

    float deadZone;
};

std::vector<Gamepad> connectedGamepads;
static int numGamepads;

/*
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
*/

// -------------------------------------------------------------------------------------------------------------------------------------------

static void associate(int a, std::string b) {
}

bool Input::startup() {
    LOG("Input subsystem startup", 0);

    showGamepadControls = false;
    firstMouseMotion = true;
    
    //  check gamepads
    int ret = SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
    if (ret != 0) {
        LOG("Error initializing SDL controller subsystem: %d", ret);
    }
    
    //  try to read gamepad mappings
    //  TODO: check local file first, fall back to archive
    DataObject dataObject = DataObject("gamecontrollerdb.txt");
    ret = SDL_GameControllerAddMappingsFromRW(SDL_RWFromMem(dataObject.getData(), dataObject.getSize()), 0);
    LOG("Added gamepad mappings: %d", ret);

    int numJoysticks = SDL_NumJoysticks();
    LOG("Num joysticks: %i", numJoysticks);
    numGamepads = 0;

    // Count how many controllers there are
    for (int i = 0; i < numJoysticks; i++) {
        if (SDL_IsGameController(i)) {
            numGamepads++;
        }
    }
    LOG("Num gamepads: %i", numGamepads);
    
    // If we have some controllers attached
    if (numGamepads > 0) {
        showGamepadControls = true;
        for (int i = 0; i < numGamepads; i++) {
            // Open the controller and add it to our list
            Gamepad gamepad;
            gamepad.gamepad = SDL_GameControllerOpen(i);
            if (SDL_GameControllerGetAttached(gamepad.gamepad) == 1) {
                SDL_Joystick *j = SDL_GameControllerGetJoystick(gamepad.gamepad);
                if (SDL_JoystickIsHaptic(j)) {
                    gamepad.haptic = SDL_HapticOpenFromJoystick(j);
                    LOG("Haptic Effects: %d", SDL_HapticNumEffects(gamepad.haptic));
                    LOG("Haptic Query: %x", SDL_HapticQuery(gamepad.haptic));
                    if (SDL_HapticRumbleSupported(gamepad.haptic)) {
                        LOG("Haptic Rumble Supported", 0);
                        if (SDL_HapticRumbleInit(gamepad.haptic) != 0) {
                            LOG("Haptic Rumble Init error: %s", SDL_GetError());
                            SDL_HapticClose(gamepad.haptic);
                            gamepad.haptic = nullptr;
                        }
                    } else {
                        LOG("Haptic Rumble Not Supported", 0);
                        SDL_HapticClose(gamepad.haptic);
                        gamepad.haptic = nullptr;
                    }
                } else {
                    gamepad.haptic = nullptr;
                }
                connectedGamepads.push_back(gamepad);
                LOG("Controller %d initialized", i);
            } else {
                LOG("Could not open gamecontroller %d: %s\n", i, SDL_GetError());
            }
        }
        SDL_GameControllerEventState(SDL_ENABLE);
    }
    SDL_JoystickEventState(SDL_ENABLE);

    connectedGamepads.resize(MAX_GAMEPADS);

    // Set the status of the controllers to "nothing is happening"
    for (int i = 0; i < numGamepads; i++) {
        for (int j = 0; j < BUTTON_MAX; j++) {
            connectedGamepads[i].buttons[j] = false;
            connectedGamepads[i].prevButtons[j] = false;
        }

        for (int j = 0; j < AXIS_MAX; j++) {
            connectedGamepads[i].rawAxis[j] = 0;
            connectedGamepads[i].axis[j] = 0.0f;
            connectedGamepads[i].prevAxis[j] = 0.0f;
        }
        
        connectedGamepads[i].deadZone = DEFAULT_DEADZONE;
    }

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

    for (int i = 0; i < numGamepads; i++) {
        for (int j = 0; j < BUTTON_MAX; j++) {
            connectedGamepads[i].prevButtons[j] = connectedGamepads[i].buttons[j];
        }
        for (int j = 0; j < AXIS_MAX; j++) {
            connectedGamepads[i].prevAxis[j] = connectedGamepads[i].axis[j];
        }
    }
    
    for (std::vector<SDL_Event>::iterator event = eventQueue.begin(); event != eventQueue.end(); event++) {
        if (event->type == SDL_KEYDOWN) {
            showGamepadControls = false;

            //  check fullscreen toggle
            if (event->key.keysym.sym == SDLK_RETURN && (event->key.keysym.mod & KMOD_ALT)) {
                script.execute("AB.onToggleFullscreen()");
            } else {
#ifdef DEBUG
                if (!console.active) {
                    //  pass to lua
                    script.execute("AB.onKeyPressed(" + toString(event->key.keysym.scancode) + ")");
                }
#endif
            }
            
            if (event->key.keysym.sym == SDLK_ESCAPE) {
                script.execute("AB.onBackPressed()");
            }
        }
        
        if (event->type == SDL_MOUSEMOTION) {
            if (firstMouseMotion) {
                firstMouseMotion = false;
            } else {
                showGamepadControls = false;
            }
            mouse.x = event->motion.x;
            mouse.y = event->motion.y;
            script.execute("AB.onMouseMoved(" + toString(mouse.x, false) + ", " + toString(mouse.y, false) + ")");
        }
        if (event->type ==  SDL_MOUSEWHEEL) {
            showGamepadControls = false;
            mouse.wheel = event->wheel.y;
            script.execute("AB.onMouseWheelMoved(" + toString(mouse.wheel, false) + ")");
        }
        
        if (event->type == SDL_MOUSEBUTTONDOWN) {
            showGamepadControls = false;
            script.execute("AB.onMousePressed(" +
                toString((int)event->button.button, false) + ", " +
                toString(event->button.x, false) + ", " +
                toString(event->button.y, false) + ")");
        }
        
        if (event->type == SDL_MOUSEBUTTONUP) {
            script.execute("AB.onMouseReleased(" +
                toString((int)event->button.button, false) + ", " +
                toString(event->button.x, false) + ", " +
                toString(event->button.y, false) + ")");
        }

        //    gamepad events

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
            LOG("Gamepad added: %d", event->cdevice.which);
            //addGamepad(event->cdevice.which);
            script.execute("AB.onGamepadConnected()");
            showGamepadControls = true;
        }
        if (event->type == SDL_JOYDEVICEREMOVED) {
            LOG("Gamepad removed: %d", event->cdevice.which);
            //removeGamepad(event->cdevice.which);
            script.execute("AB.onGamepadDisconnected()");
            // TODO: set showGamepadControls to false if last gamepad was removed
        }
        
        if (event->type == SDL_CONTROLLERBUTTONDOWN) {
            showGamepadControls = true;
            for (int i = 0; i < numGamepads; i++) {
                if (event->cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(connectedGamepads[i].gamepad))) {
                    connectedGamepads[i].buttons[event->cbutton.button] = true;
                    script.execute("AB.onGamepadPressed(" + toString(i) + "," + toString((int)event->cbutton.button) + ")");
                }                
            }
        }
        
        if (event->type == SDL_CONTROLLERBUTTONUP) {
            for (int i = 0; i < numGamepads; i++) {
                if (event->cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(connectedGamepads[i].gamepad))) {
                    connectedGamepads[i].buttons[event->cbutton.button] = false;
                    script.execute("AB.onGamepadReleased(" + toString(i) + "," + toString((int)event->cbutton.button) + ")");
                }                
            }
        }
        
        if (event->type == SDL_CONTROLLERAXISMOTION) {
            showGamepadControls = true;
            for (int i = 0; i < numGamepads; i++) {
                if (event->cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(connectedGamepads[i].gamepad))) {
                    connectedGamepads[i].rawAxis[event->caxis.axis] = event->caxis.value;
                }                
            }
        }
        
        //if (event->type == SDL_GAME) {   // say
        //}
    }

    //    calculate gamepad axis positions compensating for deadzone
    for (int i = 0; i < numGamepads; i++) {
        for (int axis = 0; axis < 2; axis++) {
            GamepadAxis horizAxis = axis == 0 ? AXIS_LEFT_X : AXIS_RIGHT_X;
            GamepadAxis vertAxis = axis == 0 ? AXIS_LEFT_Y : AXIS_RIGHT_Y;

            AB::Vec2 stickInput = AB::Vec2(connectedGamepads[i].rawAxis[horizAxis] / 32768.0f, connectedGamepads[i].rawAxis[vertAxis] / 32768.0f);
            if (AB::magnitude(stickInput) < connectedGamepads[i].deadZone) {
                stickInput = AB::Vec2(0, 0);
            } else {
                stickInput = AB::normalize(stickInput) * ((AB::magnitude(stickInput) - connectedGamepads[i].deadZone) / (1.0f - connectedGamepads[i].deadZone));
            }
            
            //connectedGamepads[i].prevAxis[horizAxis] = connectedGamepads[i].axis[horizAxis];
            //connectedGamepads[i].prevAxis[vertAxis] = connectedGamepads[i].axis[vertAxis];
            
            connectedGamepads[i].axis[horizAxis] = stickInput.x;
            connectedGamepads[i].axis[vertAxis] = stickInput.y;

        }
        
        //    update "axis buttons"
        connectedGamepads[i].buttons[BUTTON_LSTICK_UP] = connectedGamepads[i].axis[AXIS_LEFT_Y] < -0.15f;
        connectedGamepads[i].buttons[BUTTON_LSTICK_DOWN] = connectedGamepads[i].axis[AXIS_LEFT_Y] > 0.15f;
        connectedGamepads[i].buttons[BUTTON_LSTICK_LEFT] = connectedGamepads[i].axis[AXIS_LEFT_X] < -0.15f;
        connectedGamepads[i].buttons[BUTTON_LSTICK_RIGHT] = connectedGamepads[i].axis[AXIS_LEFT_X] > 0.15f;

        connectedGamepads[i].buttons[BUTTON_RSTICK_UP] = connectedGamepads[i].axis[AXIS_RIGHT_Y] < -0.15f;
        connectedGamepads[i].buttons[BUTTON_RSTICK_DOWN] = connectedGamepads[i].axis[AXIS_RIGHT_Y] > 0.15f;
        connectedGamepads[i].buttons[BUTTON_RSTICK_LEFT] = connectedGamepads[i].axis[AXIS_RIGHT_X] < -0.15f;
        connectedGamepads[i].buttons[BUTTON_RSTICK_RIGHT] = connectedGamepads[i].axis[AXIS_RIGHT_X] > 0.15f;

        // process triggers -  axis and "axis buttons"
        for (int axis = 0; axis < 2; axis++) {
            GamepadAxis trigger = axis == 0 ? AXIS_TRIGGER_LEFT : AXIS_TRIGGER_RIGHT;
            
            float triggerInput = connectedGamepads[i].rawAxis[trigger] / 32767.0f;
            
            //connectedGamepads[i].prevAxis[trigger] = connectedGamepads[i].axis[trigger];
            connectedGamepads[i].axis[trigger] = max(triggerInput - connectedGamepads[i].deadZone, 0.0f) / (1.0f - connectedGamepads[i].deadZone);
        }
        connectedGamepads[i].buttons[BUTTON_LTRIGGER] = connectedGamepads[i].axis[AXIS_TRIGGER_LEFT] > 0.15f;
        connectedGamepads[i].buttons[BUTTON_RTRIGGER] = connectedGamepads[i].axis[AXIS_TRIGGER_RIGHT] > 0.15f;

        //    call lua if any axis has been moved
        for (int axis = 0; axis < AXIS_MAX; axis++) {
            if (connectedGamepads[i].axis[axis] != connectedGamepads[i].prevAxis[axis]) {
                script.execute("AB.onGamepadAxisMoved(" + toString(i) + ", " + toString(axis) +
                    ", " + std::to_string(connectedGamepads[i].axis[axis]) + ")");
            }
        }

        //    call lua if any "axis buttons" have been, you know, "pressed"
        for (int button = BUTTON_LSTICK_UP; button < BUTTON_MAX; button++) {
            if (gamepadWasPressed(i, button)) {
                script.execute("AB.onGamepadPressed (" + toString(i) + ", " + toString(button) + ")");
            }
        }
    }

    //  update mouse
    memcpy(mouse.prevButtons, mouse.buttons, 3);

    Uint32 buttons = SDL_GetMouseState(NULL, NULL);
    mouse.buttons[0] = buttons & SDL_BUTTON_LMASK;
    mouse.buttons[1] = buttons & SDL_BUTTON_MMASK;
    mouse.buttons[2] = buttons & SDL_BUTTON_RMASK;
}

void Input::shutdown() {
    delete [] prevKeyStates;

    for (int i = 0; i < numGamepads; i++) {
        SDL_GameControllerClose(connectedGamepads[i].gamepad);

        connectedGamepads[i].gamepad = nullptr;
        connectedGamepads[i].joystick = -1;
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

Vec2 Input::getMousePosition() {
    return Vec2(mouse.x, mouse.y);
}

void Input::setMousePosition(Vec2 pos) {
    extern Window window;
    SDL_WarpMouseInWindow(window.window, pos.x, pos.y);
}

void Input::showMouseCursor(bool visible) {
    SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}

//-------------------------------------------------------- Gamepad functions -----------------------------------------------------

int Input::getNumGamepads() {
    return numGamepads;
}

bool Input::gamepadWasPressed(int gamepadIndex, int button) {
    if (gamepadIndex < 0 || gamepadIndex > numGamepads) {
        return false;
    }
    return (connectedGamepads[gamepadIndex].buttons[button] && !connectedGamepads[gamepadIndex].prevButtons[button]);
}

bool Input::gamepadIsPressed(int gamepadIndex, int button) {
    if (gamepadIndex < 0 || gamepadIndex > numGamepads) {
        return false;
    }
    return connectedGamepads[gamepadIndex].buttons[button];
}

bool Input::gamepadWasReleased(int gamepadIndex, int button) {
    if (gamepadIndex < 0 || gamepadIndex > numGamepads) {
        return false;
    }
    return (!connectedGamepads[gamepadIndex].buttons[button] && connectedGamepads[gamepadIndex].prevButtons[button]);
}

float Input::gamepadAxis(int gamepadIndex, int axis) {
    if (gamepadIndex < 0 || gamepadIndex > numGamepads) {
        return 0.0f;
    }
    return connectedGamepads[gamepadIndex].axis[axis];
}

void Input::setDeadzone(int gamepadIndex, float deadZone) {
    if (gamepadIndex >= 0 && gamepadIndex <= numGamepads) {
        connectedGamepads[gamepadIndex].deadZone = deadZone;
    } else {
        //  TODO: log warning level
        LOG("WARNING: gamepad index out of range: %d", gamepadIndex);
    }
}

void Input::vibrate(int gamepadIndex, float strength, int duration) {
    if (gamepadIndex >= 0 && gamepadIndex <= numGamepads) {
        if (connectedGamepads[gamepadIndex].haptic) {
            if (SDL_HapticRumblePlay(connectedGamepads[gamepadIndex].haptic, strength, duration) != 0) {
                LOG_EXP(SDL_GetError());
            }
        }
    }
}

//-------------------------------------------------------- Menu helper functions -----------------------------------------------------

bool Input::menuUp() {
    bool pressed = wasKeyPressed(UP);

    if (numGamepads > 0) {
        for (int i = 0; i < numGamepads; i++) {
            pressed = pressed || gamepadWasPressed(i, BUTTON_DPAD_UP) || gamepadWasPressed(i, BUTTON_LSTICK_UP);
        }
    }

    return pressed;
}

bool Input::menuDown() {
    bool pressed = wasKeyPressed(DOWN);

    if (numGamepads > 0) {
        for (int i = 0; i < numGamepads; i++) {
            pressed = pressed || gamepadWasPressed(i, BUTTON_DPAD_DOWN) || gamepadWasPressed(i, BUTTON_LSTICK_DOWN);
        }
    }
    
    return pressed;
}

bool Input::menuLeft() {
    bool pressed = wasKeyPressed(LEFT);

    if (numGamepads > 0) {
        for (int i = 0; i < numGamepads; i++) {
            pressed = pressed || gamepadWasPressed(i, BUTTON_DPAD_LEFT) || gamepadWasPressed(i, BUTTON_LSTICK_LEFT);
        }
    }
    
    return pressed;
}

bool Input::menuRight() {
    bool pressed = wasKeyPressed(RIGHT);

    if (numGamepads > 0) {
        for (int i = 0; i < numGamepads; i++) {
            pressed = pressed || gamepadWasPressed(i, BUTTON_DPAD_RIGHT) || gamepadWasPressed(i, BUTTON_LSTICK_RIGHT);
        }
    }
    
    return pressed;
}

bool Input::menuSelect() {
    bool pressed = wasKeyPressed(RETURN);

    if (numGamepads > 0) {
        for (int i = 0; i < numGamepads; i++) {
            pressed = pressed || gamepadWasPressed(i, BUTTON_A);
        }
    }
    
    return pressed;
}

bool Input::menuBack() {
    bool pressed = wasKeyPressed(ESCAPE);

    if (numGamepads > 0) {
        for (int i = 0; i < numGamepads; i++) {
            pressed = pressed || gamepadWasPressed(i, BUTTON_B);
        }
    }
    
    return pressed;
}
        
}   //  namespace

