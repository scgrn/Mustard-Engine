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

#ifndef AB_VEC2_H
#define AB_VEC2_H

namespace AB {
	
class Vec2 {
	public:
		Vec2();
		Vec2(f32 x, f32 y);

		void set(f32 x, f32 y);
		void reset();

		void normalize();
		void truncate(f32 length);
        void rotate(f32 angle);

		f32 magnitude();
		f32 dotProduct(Vec2 v);

		Vec2 operator- (const Vec2 &v) { return Vec2(x - v.x, y - v.y); }
		Vec2 operator+ (const Vec2 &v) { return Vec2(x + v.x, y + v.y); }
		Vec2 operator* (f32 scalar) { return Vec2(x * scalar, y * scalar); }
		Vec2 operator/ (f32 scalar) { return Vec2(x / scalar, y / scalar); }

		Vec2& operator-= (const Vec2 &v) { x -= v.x, y -= v.y; return *this; }
		Vec2& operator+= (const Vec2 &v) { x += v.x, y += v.y; return *this; }
		Vec2& operator*= (f32 scalar) { x *= scalar, y *= scalar; return *this; }
		Vec2& operator/= (f32 scalar) { x /= scalar, y /= scalar; return *this; }

		Vec2 operator- () { return Vec2(-x, -y); }
		bool operator== (const Vec2 &v) { return (x == v.x && y == v.y); }
		bool operator!= (const Vec2 &v) { return (x != v.x || y != v.y); }

		f32 x, y;
};

}

#endif
