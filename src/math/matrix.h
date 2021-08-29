/**

zlib License

(C) 2021 Andrew Krause

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

#ifndef AB_MAT4_H
#define AB_MAT4_H

namespace AB {
	
class Mat4 {
	public:
		Mat4() {}
		Mat4(f32[]) {}
		
		static Mat4 ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) { return Mat4(); }
		static Mat4 perspective(f32 fov, f32 aspect, f32 near, f32 far) { return Mat4(); }
		static Mat4 rotate(Mat4 source, f32 theta, Vec3 axis) { return Mat4(); }
		static Mat4 translate(Mat4 source, Vec3 offset) { return Mat4(); }
		static Mat4 scale() { return Mat4(); }
		
		
		f32 data[16];
		
};

}

#endif
