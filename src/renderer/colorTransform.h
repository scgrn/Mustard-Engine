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

#ifndef AB_COLOR_TRANSFORM_H
#define AB_COLOR_TRANSFORM_H

// thanks, Cathy! https://www.tursiopsstudios.com/dev-blog/2019/10/22/oct-2019-linear-algebra-for-sprite-coloring

namespace AB {

namespace blend {

inline Mat4 identity() {
    return Mat4();
}

inline Mat4 invert() {
    f32 matrix[] = {
        -1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, -1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, -1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

	return Mat4(matrix);
}

inline Mat4 multiply(const f32 r, const f32 g, const f32 b) {
    f32 matrix[] = {
        r, 0.0f, 0.0f, 0.0f,
        0.0f, g, 0.0f, 0.0f,
        0.0f, 0.0f, b, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    //return transpose(Mat4(matrix));
	return Mat4(matrix);
}

inline Mat4 screen(const f32 r, const f32 g, const f32 b) {
    f32 matrix[] = {
        1.0f-r, 0.0f, 0.0f, r,
        0.0f, 1.0f-g, 0.0f, g,
        0.0f, 0.0f, 1.0f-b, b,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    //return transpose(Mat4(matrix));
	return Mat4(matrix);
}

inline Mat4 colorFill(const f32 r, const f32 g, const f32 b, const f32 a) {
    f32 matrix[] = {
        1.0f-a, 0.0f, 0.0f, r*a,
        0.0f, 1.0f-a, 0.0f, g*a,
        0.0f, 0.0f, 1.0f-a, b*a,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    //return transpose(Mat4(matrix));
	return Mat4(matrix);
}

inline Mat4 linearDodge(const f32 r, const f32 g, const f32 b);
inline Mat4 linearBurn(const f32 r, const f32 g, const f32 b);
inline Mat4 colorDodge(const f32 r, const f32 g, const f32 b);
inline Mat4 colorBurn(const f32 r, const f32 g, const f32 b);

inline Mat4 tintedMonochrome(const f32 r, const f32 g, const f32 b, const f32 a) {
    f32 matrix[] = {
        1.0f-a, 0.0f, 0.0f, r*a,
        0.0f, 1.0f-a, 0.0f, g*a,
        0.0f, 0.0f, 1.0f-a, b*a,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    //return transpose(Mat4(matrix));
	return Mat4(matrix);
}

inline Mat4 hueShift(const f32 theta);

inline Mat4 saturate(const f32 v);

inline Mat4 brightness(const f32 v);
inline Mat4 darkness(const f32 v);
inline Mat4 contrast(const f32 v);

}   //  blend namespace

}   //  AB namespace

#endif // AB_COLOR_TRANSFORM_H
