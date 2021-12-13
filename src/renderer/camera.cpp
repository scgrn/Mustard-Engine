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
	projectionMatrix = ortho(left, right, bottom, top, -1.0f, 1.0f);
}

PerspectiveCamera::PerspectiveCamera() {
	projectionMatrix = Mat4();
	viewMatrix = Mat4();
}

void PerspectiveCamera::setProjection(float fov) {
    projectionMatrix = perspective(toRadians(fov), (float)window.currentMode.xRes / (float)window.currentMode.yRes, 0.1f, 1000.0f);
}

void PerspectiveCamera::setView(Mat4 view) {
	viewMatrix = view;
}

void PerspectiveCamera::recalculateViewMatrix() {
	viewMatrix = Mat4();
	
	viewMatrix = viewMatrix * rotateX(rotation.x);
	viewMatrix = viewMatrix * rotateY(rotation.y);
	viewMatrix = viewMatrix * rotateZ(rotation.z);
	
	viewMatrix = viewMatrix * translate(position);
}

Vec2 PerspectiveCamera::project(Vec3 point, Mat4 modelMatrix) {
	AB::Mat4 transform = projectionMatrix * viewMatrix * modelMatrix;
	
	AB::Vec4 p = AB::Vec4(point.x, point.y, point.z, 1.0f);
	AB::Vec4 result = transform * p;

	if (fabs(result.w) >= std::numeric_limits<float>::epsilon()) {
		result *= (1.0f / result.w);
	}
	
	result.x = (result.x + 1.0f) * 0.5f;
	result.y = (-result.y + 1.0f) * 0.5f;
	
	AB::Vec2 ret = AB::Vec2(result.x * window.currentMode.xRes, result.y * window.currentMode.yRes);
	
	return ret;
}

}
