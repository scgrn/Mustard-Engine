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
These are called from the engine during the main loop or when certain events occur
*/

/// Load configuration. Called before window and renderer are created.
// This should set a global table:
//	videoConfig = {
//		title,
//		xRes,
//		yRes,
//		fullscreen,
//		vsync
//	}	
// @function AB.loadConfig

/// Initializes application. Called once after window and renderer are created.
// @function AB.init

/// Updates logic. Called 60 times a second.
// @function AB.update

/// Renders graphics
// @function AB.render

/// Called when fullscreen mode is toggled
// @function AB.onToggleFullscreen


/// Called when application focus is lost
// @function AB.onFocusLost

/// Called when application focus is gained
// @function AB.onFocusGained

/// Called when application window is closed
// @function AB.onWindowClose


/// Called when a key is pressed
// @param key Scancode of key
// See <scancodes>
// @function AB.onKeyPressed

/// Called when a key is released
// @param key Scancode of key
// @function AB.onKeyReleased


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
// @param index Gamepad index
// @function AB.onGamepadConnected

/// Called when a gamepad is disconnected
// @param index Gamepad index
// @function AB.onGamepadDisonnected

/// Called when a gamedpad button is pressed
// @param index Gamepad index
// @param index Button index
// @function AB.onGamepadPressed

/// Called when a gamepad button is released
// @param index Gamepad index
// @param index Button index
// @function AB.onGamepadReleased

/// Called when a gamepad axis is moved
// @param index Gamepad index
// @param axis Axis that was moved
// @param delta Move amount
// @function AB.onGamepadAxisMoved
