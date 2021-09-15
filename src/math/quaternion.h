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

#ifndef AB_QUATERNION_H
#define AB_QUATERNION_H

#include "matrix.h"

namespace AB {

class Quaternion {
    public:
        Vec4 q;

        float magnitude() {
            return sqrt((q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.w * q.w));
        }

        void normalize() {
            q /= magnitude();
        }

        void fromEuler(float pitch, float yaw, float roll) {
            // Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll
            // and multiply those together.
            // the calculation below does the same, just shorter

            const float PIOVER180 = M_PI / 180.0f;

            float p = pitch * PIOVER180 / 2.0f;
            float y = yaw * PIOVER180 / 2.0f;
            float r = roll * PIOVER180 / 2.0f;

            float sinp = sin(p);
            float siny = sin(y);
            float sinr = sin(r);
            float cosp = cos(p);
            float cosy = cos(y);
            float cosr = cos(r);

            q.x = sinr * cosp * cosy - cosr * sinp * siny;
            q.y = cosr * sinp * cosy + sinr * cosp * siny;
            q.z = cosr * cosp * siny - sinr * sinp * cosy;
            q.w = cosr * cosp * cosy + sinr * sinp * siny;

            normalize();
        }

        Mat4 toMatrix() {
            Mat4 ret;

            float xx = q.x * q.x;
            float xy = q.x * q.y;
            float xz = q.x * q.z;
            float xw = q.x * q.w;

            float yy = q.y * q.y;
            float yz = q.y * q.z;
            float yw = q.y * q.w;

            float zz = q.z * q.z;
            float zw = q.z * q.w;

            ret.data2d[0][0] = 1 - 2 * (yy + zz);
            ret.data2d[1][0] =     2 * (xy - zw);
            ret.data2d[2][0] =     2 * (xz + yw);

            ret.data2d[0][1] =     2 * (xy + zw);
            ret.data2d[1][1] = 1 - 2 * (xx + zz);
            ret.data2d[2][1] =     2 * (yz - xw);

            ret.data2d[0][2] =     2 * (xz - yw);
            ret.data2d[1][2] =     2 * (yz + xw);
            ret.data2d[2][2] = 1 - 2 * (xx + yy);

            return ret;
        }
};

}   //  namespace

#endif

