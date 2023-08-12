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

#include "../pch.h"

#include "matrix.h"
#include "math.h"

namespace AB {

Mat3::Mat3(Mat4 &mat4) {
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            data2d[y][x] = mat4.data2d[y][x];
        }
    }
}

Mat4 ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
    Mat4 ret;

    f32 lr = 1.0f / (left - right);
    f32 bt = 1.0f / (bottom - top);
    f32 nf = 1.0f / (near - far);

    ret.data2d[0][0] = -2.0f * lr;
    ret.data2d[1][1] = -2.0f * bt;
    ret.data2d[2][2] = 2.0f * nf;

    ret.data2d[3][0] = (left + right) * lr;
    ret.data2d[3][1] = (top + bottom) * bt;
    ret.data2d[3][2] = (far + near) * nf;

    return ret;
}

Mat4 perspective(f32 fov, f32 aspect, f32 near, f32 far) {
    Mat4 ret;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            ret.data2d[y][x] = 0.0f;
        }
    }
        
    f32 halfTanFOV = tan(fov * 0.5f);
    ret.data2d[0][0] = 1.0f / (aspect * halfTanFOV);
    ret.data2d[1][1] = 1.0f / halfTanFOV;
    ret.data2d[2][2] = -((far + near) / (far - near));
    ret.data2d[2][3] = -1.0f;
    ret.data2d[3][2] = -((2.0f * far * near) / (far - near));

    return ret;
}

Mat4 rotate(Mat4 source, f32 theta, Vec3 axis) {
    Mat4 ret;
    
    axis = normalize(axis);

    float sinTheta = sin(toRadians(theta));
    float cosTheta = cos(toRadians(theta));
    float cosValue = 1.0f - cosTheta;

    ret.data2d[0][0] = (axis.x * axis.x * cosValue) + cosTheta;
    ret.data2d[0][1] = (axis.x * axis.y * cosValue) + (axis.z * sinTheta);
    ret.data2d[0][2] = (axis.x * axis.z * cosValue) - (axis.y * sinTheta);

    ret.data2d[1][0] = (axis.y * axis.x * cosValue) - (axis.z * sinTheta);
    ret.data2d[1][1] = (axis.y * axis.y * cosValue) + cosTheta;
    ret.data2d[1][2] = (axis.y * axis.z * cosValue) + (axis.x * sinTheta);

    ret.data2d[2][0] = (axis.z * axis.x * cosValue) + (axis.y * sinTheta);
    ret.data2d[2][1] = (axis.z * axis.y * cosValue) - (axis.x * sinTheta);
    ret.data2d[2][2] = (axis.z * axis.z * cosValue) + cosTheta;

    return (ret);
}

Mat4 translate(Vec3 offset) {
    Mat4 ret;
    
    ret.data2d[3][0] = offset.x;
    ret.data2d[3][1] = offset.y;
    ret.data2d[3][2] = offset.z;
    
    return ret;
}

Mat4 scale(Vec3 scale) {
    Mat4 ret;
    
    ret.data2d[0][0] = scale.x;
    ret.data2d[1][1] = scale.y;
    ret.data2d[2][2] = scale.z;
    
    return ret;
}

Mat4 rotateX(float theta) {
    Mat4 rot;

    rot.data2d[1][1] = cos(theta);
    rot.data2d[2][1] = -sin(theta);
    rot.data2d[1][2] = sin(theta);
    rot.data2d[2][2] = cos(theta);

    return rot;
}

Mat4 rotateY(float theta) {
    Mat4 rot;

    rot.data2d[0][0] = cos(theta);
    rot.data2d[2][0] = sin(theta);
    rot.data2d[0][2] = -sin(theta);
    rot.data2d[2][2] = cos(theta);

    return rot;
}

Mat4 rotateZ(float theta) {
    Mat4 rot;

    rot.data2d[0][0] = cos(theta);
    rot.data2d[1][0] = -sin(theta);
    rot.data2d[0][1] = sin(theta);
    rot.data2d[1][1] = cos(theta);

    return rot;
}

Mat4 transpose(Mat4 matrix) {
   Mat4 ret;

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            ret.data2d[x][y] = matrix.data2d[2 - x][2 - y];
        }
    }

    return ret;
}

Mat4 inverse(Mat4 matrix) {
    const f32* m = matrix.data1d;

    f32 t0 = m[10] * m[15];
    f32 t1 = m[14] * m[11];
    f32 t2 = m[6] * m[15];
    f32 t3 = m[14] * m[7];
    f32 t4 = m[6] * m[11];
    f32 t5 = m[10] * m[7];
    f32 t6 = m[2] * m[15];
    f32 t7 = m[14] * m[3];
    f32 t8 = m[2] * m[11];
    f32 t9 = m[10] * m[3];
    f32 t10 = m[2] * m[7];
    f32 t11 = m[6] * m[3];
    f32 t12 = m[8] * m[13];
    f32 t13 = m[12] * m[9];
    f32 t14 = m[4] * m[13];
    f32 t15 = m[12] * m[5];
    f32 t16 = m[4] * m[9];
    f32 t17 = m[8] * m[5];
    f32 t18 = m[0] * m[13];
    f32 t19 = m[12] * m[1];
    f32 t20 = m[0] * m[9];
    f32 t21 = m[8] * m[1];
    f32 t22 = m[0] * m[5];
    f32 t23 = m[4] * m[1];

    Mat4 ret;
    f32* o = ret.data1d;

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    f32 d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

    o[0] = d * o[0];
    o[1] = d * o[1];
    o[2] = d * o[2];
    o[3] = d * o[3];
    o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

    return ret;
}

Mat4 lookAt(Vec3 position, Vec3 target, Vec3 up) {
    Mat4 ret;
    Vec3 zAxis;
    
    zAxis.x = target.x - position.x;
    zAxis.y = target.y - position.y;
    zAxis.z = target.z - position.z;

    zAxis = normalize(zAxis);
    Vec3 cross = crossProduct(zAxis, up);
    Vec3 xAxis = normalize(cross);
    Vec3 yAxis = crossProduct(xAxis, zAxis);

    ret.data1d[0] = xAxis.x;
    ret.data1d[1] = yAxis.x;
    ret.data1d[2] = -zAxis.x;
    ret.data1d[3] = 0;
    ret.data1d[4] = xAxis.y;
    ret.data1d[5] = yAxis.y;
    ret.data1d[6] = -zAxis.y;
    ret.data1d[7] = 0;
    ret.data1d[8] = xAxis.z;
    ret.data1d[9] = yAxis.z;
    ret.data1d[10] = -zAxis.z;
    ret.data1d[11] = 0;
    ret.data1d[12] = -dotProduct(xAxis, position);
    ret.data1d[13] = -dotProduct(yAxis, position);
    ret.data1d[14] = dotProduct(zAxis, position);
    ret.data1d[15] = 1.0f;

    //ret = translate(Vec3(-position.x, -position.y, -position.z)) * ret;
    
    return ret;
}

Vec3 forward(Mat4 const& matrix) {
    Vec3 forward;
    forward.x = -matrix.data1d[2];
    forward.y = -matrix.data1d[6];
    forward.z = -matrix.data1d[10];

    return normalize(forward);    
}

Vec3 backward(Mat4 const& matrix) {
    Vec3 backward;
    backward.x = matrix.data1d[2];
    backward.y = matrix.data1d[6];
    backward.z = matrix.data1d[10];

    return normalize(backward);
}

Vec3 up(Mat4 const& matrix) {
    Vec3 up;
    up.x = matrix.data1d[1];
    up.y = matrix.data1d[5];
    up.z = matrix.data1d[9];

    return normalize(up);    
}

Vec3 down(Mat4 const& matrix) {
    Vec3 down;
    down.x = -matrix.data1d[1];
    down.y = -matrix.data1d[5];
    down.z = -matrix.data1d[9];

    return normalize(down);
}

Vec3 left(Mat4 const& matrix) {
    Vec3 left;
    left.x = -matrix.data1d[0];
    left.y = -matrix.data1d[4];
    left.z = -matrix.data1d[8];

    return normalize(left);
}

Vec3 right(Mat4 const& matrix) {
    Vec3 right;
    right.x = matrix.data1d[0];
    right.y = matrix.data1d[4];
    right.z = matrix.data1d[8];

    return normalize(right);
}

}

