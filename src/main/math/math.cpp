/**

zlib License

(C) 2023 Andrew Krause

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

#include "math.h"

namespace AB {

f32 distance(Vec2 a, Vec2 b) {
    return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

f32 distPointToLine(Vec2 a, Vec2 b, Vec2 c) {
    f32 l = distance(a, b);
    if (l == 0.0f) {
        return distance(a, c);
    }
    f32 r = ((c.x - a.x) * (b.x - a.x) + (c.y - a.y) * (b.y - a.y)) / (l * l);
    r = max(0.0f, min(1.0f, r));
    Vec2 p(a.x + r * (b.x - a.x), a.y + r * (b.y - a.y));

    return distance(p, c);
}

b8 lineSegmentIntersection(Vec2 a, Vec2 b, Vec2 c, Vec2 d) {
    f32 r = (a.y - c.y) * (d.x - c.x) - (a.x - c.x) * (d.y - c.y);
    f32 s = (a.y - c.y) * (b.x - a.x) - (a.x - c.x) * (b.y - a.y);

    f32 denom = (b.x - a.x) * (d.y - c.y) - (b.y - a.y) * (d.x - c.x);
    if (denom != 0.0f) {
        r /= denom;
        s /= denom;

        return r > 0.0f && r <= 1.0f && s > 0.0f && s <= 1.0f;
    } else {
        return false;
    }
}

}    // namespace

