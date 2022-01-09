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

/**

    Matrix lib in column-major format
    03.01.14

*/

// http://www.flipcode.com/documents/matrfaq.html

#ifndef AB_MATRIX_H
#define AB_MATRIX_H

#include "vector.h"
#include "../types.h"

namespace AB {

/*
//  Generic matrix
template <typename T, int rows, int cols>
struct Matrix {
    T data2d[rows][cols];

    void setIdentity() {
    }
};
*/

struct Mat4;

struct Mat3 {
    f32 data2d[3][3];

    Mat3() {
        loadIdentity();
    }
	
	Mat3(f32 data2d[]) {
		// TODO: populate!
	}

	Mat3(Mat4 &mat4);
	
    void loadIdentity() {
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3; x++) {
                data2d[y][x] = (x == y) ? 1.0f : 0.0f;
            }
        }
    }

    Vec3 operator* (Vec3 const& v) {
        Vec3 ret;

        ret.x = data2d[0][0] * v.x + data2d[1][0] * v.y + data2d[2][0] * v.z + data2d[3][0];
        ret.y = data2d[0][1] * v.x + data2d[1][1] * v.y + data2d[2][1] * v.z + data2d[3][1];
        ret.z = data2d[0][2] * v.x + data2d[1][2] * v.y + data2d[2][2] * v.z + data2d[3][2];

        return ret;
	}

	Mat3 operator* (Mat3 const& m) {
        Mat3 ret;

        for (int c = 0; c < 3; c++) {
            for (int r = 0; r < 3; r++) {
                ret.data2d[c][r] = data2d[0][r] * m.data2d[c][0] +
                            data2d[1][r] * m.data2d[c][1] +
                            data2d[2][r] * m.data2d[c][2];
            }
        }

        return ret;
    }
};

struct Mat4 {
	union {
		f32 data2d[4][4];
		f32 data1d[16];
	};
	
    Mat4() {
        loadIdentity();
    }
	
	Mat4(f32 data2d[]) {
		int index = 0;
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
				this->data2d[y][x] = data2d[index];
				index++;
			}
		}
	}

	Mat4(Mat3 m) {
		loadIdentity();
		
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3; x++) {
                data2d[y][x] = m.data2d[y][x];
            }
        }
	}

    void loadIdentity() {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                data2d[y][x] = (x == y) ? 1.0f : 0.0f;
            }
        }
    }

    Vec3 operator* (Vec3 const& v) {
        Vec3 ret;

        ret.x = data2d[0][0] * v.x + data2d[1][0] * v.y + data2d[2][0] * v.z + data2d[3][0];
        ret.y = data2d[0][1] * v.x + data2d[1][1] * v.y + data2d[2][1] * v.z + data2d[3][1];
        ret.z = data2d[0][2] * v.x + data2d[1][2] * v.y + data2d[2][2] * v.z + data2d[3][2];

        return ret;
	}

    Vec4 operator* (Vec4 const& v) {
        Vec4 ret;

        ret.x = data2d[0][0] * v.x + data2d[1][0] * v.y + data2d[2][0] * v.z + data2d[3][0];
        ret.y = data2d[0][1] * v.x + data2d[1][1] * v.y + data2d[2][1] * v.z + data2d[3][1];
        ret.z = data2d[0][2] * v.x + data2d[1][2] * v.y + data2d[2][2] * v.z + data2d[3][2];
        ret.w = data2d[0][3] * v.x + data2d[1][3] * v.y + data2d[2][3] * v.z + data2d[3][3];

        return ret;
     }

	Mat4 operator* (Mat4 const& m) {
        Mat4 ret;

        for (int c = 0; c < 4; c++) {
            for (int r = 0; r < 4; r++) {
                ret.data2d[c][r] = data2d[0][r] * m.data2d[c][0] +
                            data2d[1][r] * m.data2d[c][1] +
                            data2d[2][r] * m.data2d[c][2] +
                            data2d[3][r] * m.data2d[c][3];
            }
        }

        return ret;
    }
};

extern Mat4 ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
extern Mat4 perspective(f32 fov, f32 aspect, f32 near, f32 far);
extern Mat4 rotate(Mat4 source, f32 theta, Vec3 axis);
extern Mat4 translate(Vec3 offset);
extern Mat4 scale(Vec3 scale);

extern Mat4 rotateX(f32 theta);
extern Mat4 rotateY(f32 theta);
extern Mat4 rotateZ(f32 theta);

extern Mat4 inverse(Mat4 matrix);
extern Mat4 transpose(Mat4 matrix);
extern Mat4 lookAt(Vec3 position, Vec3 target, Vec3 up);

extern Vec3 forward(Mat4 const& matrix);
extern Vec3 backward(Mat4 const& matrix);
extern Vec3 up(Mat4 const& matrix);
extern Vec3 down(Mat4 const& matrix);
extern Vec3 left(Mat4 const& matrix);
extern Vec3 right(Mat4 const& matrix);

}   // namspace

#endif

