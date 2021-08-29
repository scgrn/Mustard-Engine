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

#include "camera.h"
#include "../core/window.h"
#include "../misc/misc.h"

namespace AB {
	
extern Window window;

OrthographicCamera::OrthographicCamera() {}

void OrthographicCamera::setProjection(float left, float right, float bottom, float top) {
	projectionMatrix = Mat4::ortho(left, right, bottom, top, -1.0f, 1.0f);
}

PerspectiveCamera::PerspectiveCamera() {
	projectionMatrix = Mat4();
	viewMatrix = Mat4();
}

void PerspectiveCamera::setProjection(float fov) {
    projectionMatrix = Mat4::perspective(toRadians(fov), (float)window.currentMode.xRes / (float)window.currentMode.yRes, 0.1f, 1000.0f);
}

void PerspectiveCamera::recalculateViewMatrix() {
	viewMatrix = Mat4();
	
	viewMatrix = Mat4::rotate(viewMatrix, rotation.x, Vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = Mat4::rotate(viewMatrix, rotation.y, Vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = Mat4::rotate(viewMatrix, rotation.z, Vec3(0.0f, 0.0f, 1.0f));
	
	viewMatrix = Mat4::translate(viewMatrix, position);
}

}
