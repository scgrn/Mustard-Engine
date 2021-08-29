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

#ifndef AB_VEC4_H
#define AB_VEC4_H

namespace AB {
	
class Vec4 {
	public:
		Vec4();
		Vec4(f32 x, f32 y, f32 z, f32 w);

		void set(f32 x, f32 y, f32 z, f32 w);
		void reset();

		void normalize();
		void truncate(f32 length);

		f32 magnitude();
		f32 dotProduct(Vec4 v);
        Vec4 crossProduct(Vec4 v);

		Vec4 operator- (const Vec4 &v) { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
		Vec4 operator+ (const Vec4 &v) { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
		Vec4 operator* (f32 scalar) { return Vec4(x * scalar, y * scalar, z * scalar, w * scalar); }
		Vec4 operator/ (f32 scalar) { return Vec4(x / scalar, y / scalar, z / scalar, w / scalar); }

		Vec4& operator-= (const Vec4 &v) { x -= v.x, y -= v.y, z -= v.z, w -= v.w; return *this; }
		Vec4& operator+= (const Vec4 &v) { x += v.x, y += v.y, z += v.z, w += v.w; return *this; }
		Vec4& operator*= (f32 scalar) { x *= scalar, y *= scalar, z *= scalar, w *= scalar; return *this; }
		Vec4& operator/= (f32 scalar) { x /= scalar, y /= scalar, z /= scalar, w /= scalar; return *this; }

		Vec4 operator- () { return Vec4(-x, -y, -z, -w); }
		bool operator== (const Vec4 &v) { return (x == v.x && y == v.y && z == v.z && w == v.w); }
		bool operator!= (const Vec4 &v) { return (x != v.x || y != v.y || z != v.z || w != v.w); }

		f32 x, y, z, w;
};

}

#endif

