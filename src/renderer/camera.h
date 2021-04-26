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

#ifndef AB_CAMERA_H
#define AB_CAMERA_H

#include <glm/glm.hpp>

namespace AB {

class Camera {
	public:
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 viewProjectionMatrix;

		virtual void recalculateViewMatrix() {}
		
	protected:

};


class OrthographicCamera : public Camera {
	public:
		OrthographicCamera();
		void setProjection(float left, float right, float bottom, float top);
};


class PerspectiveCamera : public Camera {
	public:
		PerspectiveCamera();
		void setProjection(float fov);
		virtual void recalculateViewMatrix();
		
		glm::vec3 rotation;
		glm::vec3 position;
};


}

#endif