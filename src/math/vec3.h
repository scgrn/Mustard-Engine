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

#ifndef AB_VEC3_H
#define AB_VEC3_H

namespace AB {
	
class Vec3 {
	public:
		Vec3();
		Vec3(f32 x, f32 y, f32 z);

		void set(f32 x, f32 y, f32 z);
		void reset();

		void normalize();
		void truncate(f32 length);

		f32 magnitude();
		f32 dotProduct(Vec3 v);
        Vec3 crossProduct(Vec3 v);

		Vec3 operator- (const Vec3 &v) { return Vec3(x - v.x, y - v.y, z - v.z); }
		Vec3 operator+ (const Vec3 &v) { return Vec3(x + v.x, y + v.y, z + v.z); }
		Vec3 operator* (f32 scalar) { return Vec3(x * scalar, y * scalar, z * scalar); }
		Vec3 operator/ (f32 scalar) { return Vec3(x / scalar, y / scalar, z / scalar); }

		Vec3& operator-= (const Vec3 &v) { x -= v.x, y -= v.y, z -= v.z; return *this; }
		Vec3& operator+= (const Vec3 &v) { x += v.x, y += v.y, z += v.z; return *this; }
		Vec3& operator*= (f32 scalar) { x *= scalar, y *= scalar, z *= scalar; return *this; }
		Vec3& operator/= (f32 scalar) { x /= scalar, y /= scalar, z /= scalar; return *this; }

		Vec3 operator- () { return Vec3(-x, -y, -z); }
		bool operator== (const Vec3 &v) { return (x == v.x && y == v.y && z == v.z); }
		bool operator!= (const Vec3 &v) { return (x != v.x || y != v.y || z != v.z); }

		f32 x, y, z;
};

}

#endif

