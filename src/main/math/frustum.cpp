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

#include "frustum.h"
#include "math.h"

namespace AB {

Frustum::Frustum() {}

Frustum::~Frustum() {}

void Frustum::setCamInternals(f32 angle, f32 ratio, f32 nearD, f32 farD) {
    this->ratio = ratio;
    this->angle = angle;
    this->nearD = nearD;
    this->farD = farD;

    tang = (f32)tan(toRadians(angle) * 0.5f) ;
    nh = nearD * tang;
    nw = nh * ratio; 
    fh = farD * tang;
    fw = fh * ratio;
}

void Frustum::setCamDef(Vec3 &p, Vec3 &l, Vec3 &u) {
    Vec3 dir,nc,fc,X,Y,Z;

    Z = p - l;
    Z = normalize(Z);
    
    X = crossProduct(u, Z);
    X = normalize(X);
    
    Y = crossProduct(Z, X);

    nc = p - Z * nearD;
    fc = p - Z * farD;

    ntl = nc + Y * nh - X * nw;
    ntr = nc + Y * nh + X * nw;
    nbl = nc - Y * nh - X * nw;
    nbr = nc - Y * nh + X * nw;

    ftl = fc + Y * fh - X * fw;
    ftr = fc + Y * fh + X * fw;
    fbl = fc - Y * fh - X * fw;
    fbr = fc - Y * fh + X * fw;

    pl[TOP].setPoints(ntr,ntl,ftl);
    pl[BOTTOM].setPoints(nbl,nbr,fbr);
    pl[LEFT].setPoints(ntl,nbl,fbl);
    pl[RIGHT].setPoints(nbr,ntr,fbr);
    pl[NEAR].setPoints(ntl,ntr,nbr);
    pl[FAR].setPoints(ftr,ftl,fbl);
}

i32 Frustum::pointInFrustum(Vec3 &p) {
    i32 result = INSIDE;
    for(i32 i=0; i < 6; i++) {
        if (pl[i].distance(p) < 0.0f) {
            return OUTSIDE;
        }
    }
    return(result);
}

i32 Frustum::sphereInFrustum(Vec3 &p, f32 radius) {
    i32 result = INSIDE;
    f32 distance;

    for(i32 i = 0; i < 6; i++) {
        distance = pl[i].distance(p);
        if (distance < -radius) {
            return OUTSIDE;
        } else if (distance < radius) {
            result = INTERSECT;
        }
    }
    return(result);
}

i32 Frustum::boxInFrustum(AABB &box) {
    i32 result = INSIDE;
    for(i32 i = 0; i < 6; i++) {
        Vec3 vertexP = box.getVertexP(pl[i].normal); 
        if (pl[i].distance(vertexP) < 0.0f) {
            return OUTSIDE;
        } else {
            Vec3 vertexN = box.getVertexN(pl[i].normal);
            if (pl[i].distance(vertexN) < 0.0f) {
                result =  INTERSECT;
            }
        }
    }
    return(result);
}

}   //  namespace

