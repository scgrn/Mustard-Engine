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

#include "../pch.h"

#include "sprite.h"
#include "image.h"
#include "../misc/misc.h"
#include "renderer.h"

namespace AB {

extern Renderer renderer;

Sprite::Sprite() {
    image = NULL;
    texture = NULL;
    collisionMask = NULL;
}

void Sprite::load(std::string const& filename) {
    if (filename != ".") {
        LOG("Loading <%s>", filename.c_str());

        image = new Image(filename.c_str());

        width = image->width;
        height = image->height;
        halfX = width / 2;
        halfY = height / 2;
        radius = sqrt((float)((halfX * halfX) + (halfY * halfY)));
    }
}

void Sprite::release() {
    if (image) {
        delete image;
        image = NULL;
    }

    texture.reset();

    if (collisionMask) {
        delete collisionMask;
        collisionMask = NULL;
    }
}

void Sprite::buildCollisionMask(int offsetX, int offsetY) {
    LOG("Building collision mask: DIM(%d, %d), OFFSET(%d, %d)", width, height, offsetX, offsetY);
    
    if (collisionMask) {
        return;
    }

    if (!image) {
        ERR("Image data not loaded!", 0);
    }

    collisionMask = new bool[width * height];
    memset(collisionMask, 0, width * height);

    unsigned char *imageData = image->data;

    for (int y = offsetY; y < height + offsetY; y++) {
        for (int x = offsetX; x < width + offsetX; x++) {
            //  set true if alpha channel for pixel is above threshold
            if (imageData[((height - y - 1) * width + x) * 4 + 3] > 128) {
                collisionMask[y * width + x] = true;
            }
        }
    }
}

void Sprite::uploadToGPU(bool retainImage) {
    texture = std::make_shared<Texture>(image);

    u1 = 0.0f;
    v1 = 0.0f;
    u2 = texture->u2;
    v2 = texture->v2;

    if (!retainImage) {
        if (image) {
            delete image;
            image = NULL;
        }
    }
}

void Sprite::adopt(std::shared_ptr<Texture> texture, float u1, float v1, float u2, float v2, bool retainImage) {
    this->texture = texture;
    this->u1 = u1;
    this->v1 = v1;
    this->u2 = u2;
    this->v2 = v2;

    if (!retainImage) {
        if (image) {
            delete image;
            image = NULL;
        }
    }
}

void Sprite::render(RenderLayer *renderer, Vec3 pos, float rotation, Vec2 scale, Vec4 color) {
    if (texture.get() == 0) {
        uploadToGPU();
    }
    
    RenderLayer::Quad quad;
    
    quad.pos = pos;
    quad.size = Vec2(width, height);
    quad.scale = scale;
    quad.rotation = rotation;
    quad.uv = Vec4(u1, v1, u2, v2);
    quad.textureID = texture->glHandle;
    quad.color = color;

    renderer->renderQuad(quad);
}

struct Scan {
    float x1, u1, v1, x2, u2, v2;
};

void scanEdge(Vec2 p1, float u1, float v1, Vec2 p2,
    float u2, float v2, Scan *scan, int range, float yMin) {

    if (p1.y == p2.y) return;   // hey! this a horizontal slice!! beat it!

    // need swap?
    if (p2.y < p1.y) {
        // need swap!
        std::swap(p1, p2);
        std::swap(u1, u2);
        std::swap(v1, v2);
    }

    float xStep = (p2.x - p1.x) / (p2.y - p1.y);
    float uStep = (u2 - u1) / (p2.y - p1.y);
    float vStep = (v2 - v1) / (p2.y - p1.y);

    int start = max(0, (int)(p1.y - yMin));
    int stop = min(range, (int)(p2.y - yMin));

    float diff = max(0.0f, (yMin - p1.y));
    float xPos = p1.x + (diff * xStep);
    float uPos = u1 + (diff * uStep);
    float vPos = v1 + (diff * vStep);

    for (int index = start; index < stop; index++) {
        if (xPos < scan[index].x1) {
            scan[index].x1 = xPos;
            scan[index].u1 = uPos;
            scan[index].v1 = vPos;
        }

        if (xPos > scan[index].x2) {
            scan[index].x2 = xPos;
            scan[index].u2 = uPos;
            scan[index].v2 = vPos;
        }

        xPos += xStep;
        uPos += uStep;
        vPos += vStep;
    }
}

bool collides(Sprite *s1, Vec2 pos1, float angle1, float scaleX1, float scaleY1,
    Sprite *s2, Vec2 pos2, float angle2, float scaleX2, float scaleY2) {

    //  broad phase squared distance check
    float dist = (pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y);
    float sumRadii = abs(s1->radius * max(scaleX1, scaleY1)) + abs(s2->radius * max(scaleX2, scaleY2));

    if (dist <= (sumRadii * sumRadii)) {
        //  optimization.. if both scales are > 1, divide both scales by smaller scale
        //  (make smaller scale == 1) ...also modify positions
        if (scaleX1 > 1.0f && scaleY1 > 1.0f && scaleX2 > 1.0f && scaleY2) {
            float smallerXScale = min(abs(scaleX1), abs(scaleX2));
            float smallerYScale = min(abs(scaleY1), abs(scaleY2));
            float smallerScale = min(smallerXScale, smallerYScale);

            scaleX1 /= smallerScale;
            scaleY1 /= smallerScale;
            scaleX2 /= smallerScale;
            scaleY2 /= smallerScale;

            pos1 /= smallerScale;
            pos2 /= smallerScale;
        }

        //  rotate and scale bounding rectangles
        Vec2 v1[4], v2[4];

        //  first sprite
        v1[0] = Vec2(-s1->halfX, -s1->halfY);
        v1[1] = Vec2( s1->halfX, -s1->halfY);
        v1[2] = Vec2( s1->halfX,  s1->halfY);
        v1[3] = Vec2(-s1->halfX,  s1->halfY);

        float ta = toRadians(-angle1);

        for (int i = 0; i < 4; i++) {
            Vec2 scaled = Vec2(v1[i].x * scaleX1, v1[i].y * scaleY1);
            Vec2 rotated;

            rotated.x = scaled.y * sin(ta) + scaled.x * cos(ta);
            rotated.y = scaled.y * cos(ta) - scaled.x * sin(ta);

            v1[i] = rotated + pos1;
        }

        //  second sprite
        v2[0] = Vec2(-s2->halfX, -s2->halfY);
        v2[1] = Vec2( s2->halfX, -s2->halfY);
        v2[2] = Vec2( s2->halfX,  s2->halfY);
        v2[3] = Vec2(-s2->halfX,  s2->halfY);

        ta = toRadians(-angle2);

        for (int i = 0; i < 4; i++) {
            Vec2 scaled = Vec2(v2[i].x * scaleX2, v2[i].y * scaleY2);
            Vec2 rotated;

            rotated.x = scaled.y * sin(ta) + scaled.x * cos(ta);
            rotated.y = scaled.y * cos(ta) - scaled.x * sin(ta);

            v2[i] = rotated + pos2;
        }

        //  initialize vertical extents to ludicrous values
        float yMin1 = FLT_MAX;
        float yMax1 = -FLT_MAX;
        float yMin2 = FLT_MAX;
        float yMax2 = -FLT_MAX;

        //  find vertical extents of both bounding boxes
        for (int i = 0; i < 4; i++) {
            if (v1[i].y < yMin1) yMin1 = v1[i].y;
            if (v1[i].y > yMax1) yMax1 = v1[i].y;

            if (v2[i].y < yMin2) yMin2 = v2[i].y;
            if (v2[i].y > yMax2) yMax2 = v2[i].y;
        }

        //  find vertical overlap, early out if none
        if (yMax1 < yMin2 || yMax2 < yMin1) {
            return false;
        }

        //  find extent of vertical overlap
        float yMin = max(yMin1, yMin2);
        float yMax = min(yMax1, yMax2);

        //  scan convert bounding boxes
        int range = (int)(yMax - yMin);     // height of vertical overlap

        //  adding yMin to array indices will yield actual screen space y value
        Scan scan1[range], scan2[range];

        //  again, initialize to preposterous values
        for (int i = 0; i < range; i++) {
            scan1[i].x1 = FLT_MAX;
            scan1[i].x2 = -FLT_MAX;
            scan2[i].x1 = FLT_MAX;
            scan2[i].x2 = -FLT_MAX;
        }

        //  initialize u/v extents based on flippage
        float s1u1, s1v1, s1u2, s1v2, s2u1, s2v1, s2u2, s2v2;

        if (scaleX1 < 0.0f) {
            s1u1 = 1.0f;
            s1u2 = 0.0f;
        } else {
            s1u1 = 0.0f;
            s1u2 = 1.0f;
        }
        if (scaleY1 < 0.0f) {
            s1v1 = 1.0f;
            s1v2 = 0.0f;
        } else {
            s1v1 = 0.0f;
            s1v2 = 1.0f;
        }

        if (scaleX2 < 0.0f) {
            s2u1 = 1.0f;
            s2u2 = 0.0f;
        } else {
            s2u1 = 0.0f;
            s2u2 = 1.0f;
        }
        if (scaleY2 < 0.0f) {
            s2v1 = 1.0f;
            s2v2 = 0.0f;
        } else {
            s2v1 = 0.0f;
            s2v2 = 1.0f;
        }

        //  here we can skip the scan if the horizontal extents do not overlap.
        scanEdge(v1[0], s1u1, s1v1, v1[1], s1u2, s1v1, &scan1[0], range, yMin);
        scanEdge(v1[1], s1u2, s1v1, v1[2], s1u2, s1v2, &scan1[0], range, yMin);
        scanEdge(v1[2], s1u2, s1v2, v1[3], s1u1, s1v2, &scan1[0], range, yMin);
        scanEdge(v1[3], s1u1, s1v2, v1[0], s1u1, s1v1, &scan1[0], range, yMin);

        scanEdge(v2[0], s2u1, s2v1, v2[1], s2u2, s2v1, &scan2[0], range, yMin);
        scanEdge(v2[1], s2u2, s2v1, v2[2], s2u2, s2v2, &scan2[0], range, yMin);
        scanEdge(v2[2], s2u2, s2v2, v2[3], s2u1, s2v2, &scan2[0], range, yMin);
        scanEdge(v2[3], s2u1, s2v2, v2[0], s2u1, s2v1, &scan2[0], range, yMin);

        //  u/v coords were scanned at [0..1], scale them to actual size
        for (int i = 0; i < range; i++) {
            scan1[i].u1 *= s1->width;
            scan1[i].v1 *= s1->height;
            scan1[i].u2 *= s1->width;
            scan1[i].v2 *= s1->height;

            scan2[i].u1 *= s2->width;
            scan2[i].v1 *= s2->height;
            scan2[i].u2 *= s2->width;
            scan2[i].v2 *= s2->height;
        }

        for (int i = 0; i < range; i++) {
            if (scan1[i].x1 <= scan2[i].x2 && scan2[i].x1 <= scan1[i].x2) {
                if ((scan1[i].x2 - scan1[i].x1 < 1.0f) || (scan2[i].x2 - scan2[i].x1 < 1.0f)) {
                    //  not a scan! beat it!
                    continue;
                }

                //  extends of shared scan
                float x1 = max(scan1[i].x1, scan2[i].x1);
                float x2 = min(scan1[i].x2, scan2[i].x2);

                //  perform horizontal scan
                float uStep1 = (scan1[i].u2 - scan1[i].u1) / (scan1[i].x2 - scan1[i].x1);
                float vStep1 = (scan1[i].v2 - scan1[i].v1) / (scan1[i].x2 - scan1[i].x1);
                float uPos1 = scan1[i].u1 + ((x1 - scan1[i].x1) * uStep1);
                float vPos1 = scan1[i].v1 + ((x1 - scan1[i].x1) * vStep1);

                float uStep2 = (scan2[i].u2 - scan2[i].u1) / (scan2[i].x2 - scan2[i].x1);
                float vStep2 = (scan2[i].v2 - scan2[i].v1) / (scan2[i].x2 - scan2[i].x1);
                float uPos2 = scan2[i].u1 + ((x1 - scan2[i].x1) * uStep2);
                float vPos2 = scan2[i].v1 + ((x1 - scan2[i].x1) * vStep2);

                for (int x = (int)x1; x <= (int)x2; x++) {

                    //  this lame and slow! correct with sub-pixel accuracy.
                    int iUpos1 = (int)uPos1;
                    iUpos1 = clamp(iUpos1, 0, s1->width - 1);
                    int iVpos1 = (int)vPos1;
                    iVpos1 = clamp(iVpos1, 0, s1->height - 1);

                    int iUpos2 = (int)uPos2;
                    iUpos2 = clamp(iUpos2, 0, s2->width - 1);
                    int iVpos2 = (int)vPos2;
                    iVpos2 = clamp(iVpos2, 0, s2->height - 1);

                    if (s1->collisionMask[iVpos1 * s1->width + iUpos1] &&
                        s2->collisionMask[iVpos2 * s2->width + iUpos2]) {

                        return true;
                    }

                    uPos1 += uStep1;
                    vPos1 += vStep1;

                    uPos2 += uStep2;
                    vPos2 += vStep2;
                }
            }
        }
        return false;
    }

    return false;
}

}

