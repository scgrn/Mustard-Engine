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

// http://www.reedbeta.com/blog/2013/12/28/on-vector-math-libraries/

#ifndef AB_VECTOR_H
#define AB_VECTOR_H

#define _USE_MATH_DEFINES
#include <cmath>

#include "types.h"

namespace AB {

struct Mat4;

//  Generic vector
template <typename T, i32 n> struct Vector {
    T data[n];

    //  Subscript
    T& operator[](i32 i) { return data[i]; }
    const T& operator[](i32 i) const { return data[i]; }

    //  Addition, subtraction, multiplication, division
    Vector<T, n> operator- (const Vector<T, n> &v) {
        Vector<T, n> ret;
        for (i32 i = 0; i < n; i++) {
            ret[i] = data[i] - v[i];
        }
        return ret;
    }

    Vector<T, n> operator+ (const Vector<T, n> &v) {
        Vector<T, n> ret;
        for (i32 i = 0; i < n; i++) {
            ret[i] = data[i] + v[i];
        }
        return ret;
    }

    Vector<T, n> operator* (f32 const& scalar) {
        Vector<T, n> ret;
        for (i32 i = 0; i < n; i++) {
            ret[i] = data[i] * scalar;
        }
        return ret;
    }

    Vector<T, n> operator/ (f32 const& scalar) {
        Vector<T, n> ret;
        for (i32 i = 0; i < n; i++) {
            ret[i] = data[i] / scalar;
        }
        return ret;
    }

    //  Assignment operators
    Vector<T, n>& operator-= (const Vector<T, n> &v) {
        for (i32 i = 0; i < n; i++) {
            data[i] -= v[i];
        }
        return *this;
    }

    Vector<T, n>& operator+= (const Vector<T, n> &v) {
        for (i32 i = 0; i < n; i++) {
            data[i] += v[i];
        }
        return *this;
    }

    Vector<T, n>& operator*= (f32 const& scalar) {
        for (i32 i = 0; i < n; i++) {
            data[i] *= scalar;
        }
        return *this;
    }

    Vector<T, n>& operator/= (f32 const& scalar) {
        for (i32 i = 0; i < n; i++) {
            data[i] /= scalar;
        }
        return *this;
    }

    //  Unary negation, equality, inequality
    Vector<T, n> operator- () {
        Vector<T, n> ret;
        for (i32 i = 0; i < n; i++) {
            ret[i] = -data[i];
        }
        return ret;
    }

    bool operator== (const Vector<T, n> &v) {
        for (i32 i = 0; i < n; i++) {
            if (data[i] != v[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!= (const Vector<T, n> &v) {
        for (i32 i = 0; i < n; i++) {
            if (data[i] == v[i]) {
                return false;
            }
        }
        return true;
    }
};

//  Template specializations for 2, 3, and 4 component vectors

// ------------------------------------------------------------------------ Vec2 --------------------------------------------------
template <typename T> struct Vector<T, 2> {
    union {
        T data[2];
        struct { T x, y; };
        struct { T u, v; };
    };

    Vector<T, 2>() : x(0), y(0) {}
    Vector<T, 2>(T x, T y) : x(x), y(y) {}
    Vector<T, 2>(Vector<T, 3> v) : x(v.x), y(v.y) {}
    Vector<T, 2>(Vector<T, 4> v) : x(v.x), y(v.y) {}
    
    //  Subscript
    T& operator[](i32 i) { return data[i]; }
    const T& operator[](i32 i) const { return data[i]; }

    //  Addition, subtraction, multiplication, division
    Vector<T, 2> operator- (const Vector<T, 2> &v) {
        Vector<T, 2> ret;
        ret[0] = data[0] - v[0];
        ret[1] = data[1] - v[1];
        return ret;
    }

    Vector<T, 2> operator+ (const Vector<T, 2> &v) {
        Vector<T, 2> ret;
        ret[0] = data[0] + v[0];
        ret[1] = data[1] + v[1];
        return ret;
    }

    Vector<T, 2> operator* (f32 const& scalar) {
        Vector<T, 2> ret;
        ret[0] = data[0] * scalar;
        ret[1] = data[1] * scalar;
        return ret;
    }

    Vector<T, 2> operator/ (f32 const& scalar) {
        Vector<T, 2> ret;
        ret[0] = data[0] / scalar;
        ret[1] = data[1] / scalar;
        return ret;
    }

    //  Assignment operators
    Vector<T, 2>& operator-= (const Vector<T, 2> &v) {
        data[0] -= v[0];
        data[1] -= v[1];
        return *this;
    }

    Vector<T, 2>& operator+= (const Vector<T, 2> &v) {
        data[0] += v[0];
        data[1] += v[1];
        return *this;
    }

    Vector<T, 2>& operator*= (f32 const& scalar) {
        data[0] *= scalar;
        data[1] *= scalar;
        return *this;
    }

    Vector<T, 2>& operator/= (f32 const& scalar) {
        data[0] /= scalar;
        data[1] /= scalar;
        return *this;
    }

    //  Unary negation, equality, inequality
    Vector<T, 2> operator- () {
        Vector<T, 2> ret;
        ret[0] = -data[0];
        ret[1] = -data[1];
        return ret;
    }

    bool operator== (const Vector<T, 2> &v) {
        return (data[0] == v[0] && data[1] == v[1]);
    }

    bool operator!= (const Vector<T, 2> &v) {
        return (data[0] != v[0] || data[1] != v[1]);
    }
};

// ------------------------------------------------------------------------ Vec3 --------------------------------------------------
template <typename T> struct Vector<T, 3> {
    union {
        T data[3];
        struct { T x, y, z; };
        struct { T r, g, b; };
        struct { T pitch, yaw, roll; };
    };

    Vector<T, 3>() : x(0), y(0), z(0) {}
    Vector<T, 3>(T x, T y, T z) : x(x), y(y), z(z) {}
    Vector<T, 3>(Vector<T, 2> v) : x(v.x), y(v.y), z(0) {}
    Vector<T, 3>(Vector<T, 4> v) : x(v.x), y(v.y), z(v.z) {}

    //  Subscript
    T& operator[](i32 i) { return data[i]; }
    const T& operator[](i32 i) const { return data[i]; }

    //  Addition, subtraction, multiplication, division
    Vector<T, 3> operator- (const Vector<T, 3> &v) {
        Vector<T, 3> ret;
        ret[0] = data[0] - v[0];
        ret[1] = data[1] - v[1];
        ret[2] = data[2] - v[2];
        return ret;
    }

    Vector<T, 3> operator+ (const Vector<T, 3> &v) {
        Vector<T, 3> ret;
        ret[0] = data[0] + v[0];
        ret[1] = data[1] + v[1];
        ret[2] = data[2] + v[2];
        return ret;
    }

    Vector<T, 3> operator* (f32 const& scalar) {
        Vector<T, 3> ret;
        ret[0] = data[0] * scalar;
        ret[1] = data[1] * scalar;
        ret[2] = data[2] * scalar;
        return ret;
    }

    Vector<T, 3> operator* (Mat4 &m) {
        Vector<T, 3> v(x, y, z);
        return m * v;
    }

    Vector<T, 3> operator/ (f32 const& scalar) {
        Vector<T, 3> ret;
        ret[0] = data[0] / scalar;
        ret[1] = data[1] / scalar;
        ret[2] = data[2] / scalar;
        return ret;
    }

    //  Assignment operators
    Vector<T, 3>& operator-= (const Vector<T, 3> &v) {
        data[0] -= v[0];
        data[1] -= v[1];
        data[2] -= v[2];
        return *this;
    }

    Vector<T, 3>& operator+= (const Vector<T, 3> &v) {
        data[0] += v[0];
        data[1] += v[1];
        data[2] += v[2];
        return *this;
    }

    Vector<T, 3>& operator*= (f32 const& scalar) {
        data[0] *= scalar;
        data[1] *= scalar;
        data[2] *= scalar;
        return *this;
    }

    Vector<T, 3>& operator/= (f32 const& scalar) {
        data[0] /= scalar;
        data[1] /= scalar;
        data[2] /= scalar;
        return *this;
    }

    //  Unary negation, equality, inequality
    Vector<T, 3> operator- () {
        Vector<T, 3> ret;
        ret[0] = -data[0];
        ret[1] = -data[1];
        ret[2] = -data[2];
        return ret;
    }

    bool operator== (const Vector<T, 3> &v) {
        return (data[0] == v[0] && data[1] == v[1] && data[2] == v[2]);
    }

    bool operator!= (const Vector<T, 3> &v) {
        return (data[0] != v[0] || data[1] != v[1] || data[2] != v[2]);
    }
};

// ------------------------------------------------------------------------ Vec4 --------------------------------------------------
template <typename T> struct Vector<T, 4> {
    union {
        T data[4];
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
    };

    //  Subscript
    T& operator[](i32 i) { return data[i]; }
    const T& operator[](i32 i) const { return data[i]; }

    Vector<T, 4>() : x(0), y(0), z(0), w(0) {}
    Vector<T, 4>(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    Vector<T, 4>(Vector<T, 2> v) : x(v.x), y(v.y), z(0), w(0) {}
    Vector<T, 4>(Vector<T, 3> v) : x(v.x), y(v.y), z(v.z), w(0) {}
    
    //  Addition, subtraction, multiplication, division
    Vector<T, 4> operator- (const Vector<T, 4> &v) {
        Vector<T, 4> ret;
        ret[0] = data[0] - v[0];
        ret[1] = data[1] - v[1];
        ret[2] = data[2] - v[2];
        ret[3] = data[3] - v[3];
        return ret;
    }

    Vector<T, 4> operator+ (const Vector<T, 4> &v) {
        Vector<T, 4> ret;
        ret[0] = data[0] + v[0];
        ret[1] = data[1] + v[1];
        ret[2] = data[2] + v[2];
        ret[3] = data[3] + v[3];
        return ret;
    }

    Vector<T, 4> operator* (f32 const& scalar) {
        Vector<T, 4> ret;
        ret[0] = data[0] * scalar;
        ret[1] = data[1] * scalar;
        ret[2] = data[2] * scalar;
        ret[3] = data[3] * scalar;
        return ret;
    }

    Vector<T, 4> operator/ (f32 const& scalar) {
        Vector<T, 4> ret;
        ret[0] = data[0] / scalar;
        ret[1] = data[1] / scalar;
        ret[2] = data[2] / scalar;
        ret[3] = data[3] / scalar;
        return ret;
    }

    //  Assignment operators
    Vector<T, 4>& operator-= (const Vector<T, 4> &v) {
        data[0] -= v[0];
        data[1] -= v[1];
        data[2] -= v[2];
        data[3] -= v[3];
        return *this;
    }

    Vector<T, 4>& operator+= (const Vector<T, 4> &v) {
        data[0] += v[0];
        data[1] += v[1];
        data[2] += v[2];
        data[3] += v[3];
        return *this;
    }

    Vector<T, 4>& operator*= (f32 const& scalar) {
        data[0] *= scalar;
        data[1] *= scalar;
        data[2] *= scalar;
        data[3] *= scalar;
        return *this;
    }

    Vector<T, 4>& operator/= (f32 const& scalar) {
        data[0] /= scalar;
        data[1] /= scalar;
        data[2] /= scalar;
        data[3] /= scalar;
        return *this;
    }

    //  Unary negation, equality, inequality
    Vector<T, 4> operator- () {
        Vector<T, 4> ret;
        ret[0] = -data[0];
        ret[1] = -data[1];
        ret[2] = -data[2];
        ret[3] = -data[3];
        return ret;
    }

    bool operator== (const Vector<T, 4> &v) {
        return (data[0] == v[0] && data[1] == v[1] && data[2] == v[2] && data[3] == v[3]);
    }

    bool operator!= (const Vector<T, 4> &v) {
        return (data[0] != v[0] || data[1] != v[1] || data[2] != v[2] || data[3] != v[3]);
    }
};

typedef Vector<f32, 2> Vec2;
typedef Vector<f32, 3> Vec3;
typedef Vector<f32, 4> Vec4;

typedef Vector<i32, 2> Vec2i;
typedef Vector<i32, 3> Vec3i;
typedef Vector<i32, 4> Vec4i;   // yagni?

template <typename T, i32 n>
f32 magnitude(Vector<T, n> const& v) {
    f32 sum = 0;
    for (i32 i = 0; i < n; i++) {
        sum += (v[i] * v[i]);
    }
    return sqrt(sum);
}

template <typename T, i32 n>
Vector<T, n> normalize(Vector<T, n> &v) {
    f32 l = magnitude(v);

    if (l != 0.0f) {
        Vector<T, n> ret = v / l;
        return ret;
    }

    return v;
}

template <typename T, i32 n>
Vector<T, n> truncate(Vector<T, n> &v, f32 const& length) {
    f32 l = magnitude(v);

    if (l > length) {
        Vector<T, n> ret = v / l;
        ret *= length;
        return ret;
    }

    return v;
}

template <typename T, i32 n>
T dotProduct(Vector<T, n> &a, Vector<T, n> &b) {
    T ret = 0;
    for (i32 i = 0; i < n; i++) {
        ret += (a[i] * b[i]);
    }
    return ret;
}

extern Vec3 crossProduct(Vec3 const& a, Vec3 const& b);

}   // namespace

#endif

