/*

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

*/

/**
These are called from the engine during the main loop or when system events occur
*/

/// Load configuration. Called before window and renderer are created.
// This should set a global table:
//    videoConfig = {
//        title,
//        xRes,
//        yRes,
//        fullscreen,
//        vsync
//    }    
// @function AB.loadConfig

/// Called once before the main loop is entered.
// All subsystems are guaranteed to be stood up.
// @function AB.init

/// Updates logic. Called 60 times a second.
// @function AB.update

/// Renders graphics
// @function AB.render

/// Called when fullscreen mode is toggled
// @function AB.onToggleFullscreen


/// <br/>

/// Called when application focus is lost
// @function AB.onFocusLost

/// Called when application focus is gained
// @function AB.onFocusGained

/// Called when application window is closed
// @function AB.onWindowClose


/// Called when a key is pressed
// @function AB.onKeyPressed
// @param key Scancode of key
// @see input.scanCodes

/// Called when a key is released
// @function AB.onKeyReleased
// @param key Scancode of key
// @see input.scanCodes

/// Called when back is pressed on mobile (or Esc on desktop/web)
// @function AB.onBackPressed

/// Called when screen is touched on mobile
// @function AB.onTouchPressed
// @param index The touch index for multi-touch
// @param x X-coordinate
// @param y Y-coordinate

/// Called when screen is released on mobile
// @function AB.onTouchReleased
// @param index The touch index for multi-touch
// @param x X-coordinate
// @param y Y-coordinate

/// Called when a screen press is moved on mobile
// @function AB.onTouchMoved
// @param index The touch index for multi-touch
// @param x X-coordinate
// @param y Y-coordinate


/// Called when a mouse button is pressed
// @function AB.onMousePressed
// @param index Button index
// @param x X-coordinate
// @param y Y-coordinate

/// Called when a mouse button is released
// @function AB.onMouseReleased
// @param index Button index
// @param x X-coordinate
// @param y Y-coordinate

/// Called when the mouse is moved
// @function AB.onMouseMoved
// @param x X-coordinate
// @param y Y-coordinate

/// Called when the mouse wheel is moved
// @function AB.onMouseWheelMoved
// @param delta Move amount

/// Called when a gamepad is connected
// @function AB.onGamepadConnected
// @param index Gamepad index

/// Called when a gamepad is disconnected
// @function AB.onGamepadDisonnected
// @param index Gamepad index

/// Called when a gamedpad button is pressed
// @function AB.onGamepadPressed
// @param index Gamepad index
// @param button Button index
// @see input.gamepadButtons

/// Called when a gamepad button is released
// @function AB.onGamepadReleased
// @param index Gamepad index
// @param button Button index
// @see input.gamepadButtons

/// Called when a gamepad axis is moved
// @function AB.onGamepadAxisMoved
// @param index Gamepad index
// @param axis Axis that was moved
// @param position Axis position
// @see input.gamepadAxes
