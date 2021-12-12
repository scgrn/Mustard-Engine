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

//  http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
//  http://www.gamedev.net/topic/65558-gluproject-glunproject-use-examples/
Vec2 PerspectiveCamera::project(Vec3 point) {
	Vec3 ret;
	
    point.z += 1.0f;	// TODO: why

    //	transformation vectors
    float temp[8];
	
    //	modelview transform
    temp[0] = viewMatrix.data1d[0] * point.x + viewMatrix.data1d[4] * point.y + viewMatrix.data1d[8] * point.z + viewMatrix.data1d[12];  //w is always 1
    temp[1] = viewMatrix.data1d[1] * point.x + viewMatrix.data1d[5] * point.y + viewMatrix.data1d[9] * point.z + viewMatrix.data1d[13];
    temp[2] = viewMatrix.data1d[2] * point.x + viewMatrix.data1d[6] * point.y + viewMatrix.data1d[10] * point.z + viewMatrix.data1d[14];
    temp[3] = viewMatrix.data1d[3] * point.x + viewMatrix.data1d[7] * point.y + viewMatrix.data1d[11] * point.z + viewMatrix.data1d[15];
    
	//	projection transform, the final row of projection matrix is always [0 0 -1 0]
    //	so we optimize for that.
    temp[4] = projectionMatrix.data1d[0] * temp[0] + projectionMatrix.data1d[4] * temp[1] + projectionMatrix.data1d[8] * temp[2] + projectionMatrix.data1d[12] * temp[3];
    temp[5] = projectionMatrix.data1d[1] * temp[0] + projectionMatrix.data1d[5] * temp[1] + projectionMatrix.data1d[9] * temp[2] + projectionMatrix.data1d[13] * temp[3];
    temp[6] = projectionMatrix.data1d[2] * temp[0] + projectionMatrix.data1d[6] * temp[1] + projectionMatrix.data1d[10] * temp[2] + projectionMatrix.data1d[14] * temp[3];
    temp[7] = -temp[2];
    
	//	the result normalizes between -1 and 1
    if (temp[7] == 0.0)	{ //	the w value
		return ret; // fail
	}
    temp[7] = 1.0 / temp[7];
    
	//	perspective division
    temp[4] *= temp[7];
    temp[5] *= temp[7];
    temp[6] *= temp[7];
    
	//	window coordinates
    //	map x, y to range 0-1
	GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
	
    ret.x = (temp[4] * 0.5 + 0.5) * viewport[2] + viewport[0];
    ret.y = window.currentMode.yRes - ((temp[5] * 0.5 + 0.5) * viewport[3] + viewport[1]);
    
	//This is only correct when glDepthRange(0.0, 1.0)
    // windowCoordinate[2]=(1.0+fTempo[6])*0.5;	//Between 0 and 1
	
	return ret;
}

}
