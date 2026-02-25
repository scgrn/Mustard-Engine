/**

zlib License

(C) 2026 Andrew Krause

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

#ifndef AB_PARTICLE_SYSTEM_H
#define AB_PARTICLE_SYSTEM_H

#include "../math/vector.h"
#include "renderLayer.h"
#include "camera.h"

namespace AB {

struct ParticleParameters {
    Vec3 pos, vel, acceleration;
    f32 dampening = 1.0f;
    Vec4 startColor = AB::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    Vec3 endColor = AB::Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    f32 startScale, endScale;
    f32 minAngle, maxAngle;
    f32 minRotation, maxRotation;
    u32 startSprite, endSprite;
    u32 minLifetime, maxLifetime;
};

class ParticleSystem {
    public:
        ParticleSystem(u32 maxParticles = 512);

        void emit(ParticleParameters const& parameters);
        void update();
        void render(RenderLayer *renderLayer, PerspectiveCamera const& camera);

        b8 active;

    protected:
        struct Particle {
            Vec3 pos, vel, acceleration;
            f32 dampening;
            Vec4 startColor, endColor;
            f32 startScale, endScale;
            f32 angle, rotation;
            i32 startSprite, endSprite;
            i32 lifetime, lifetimeRemaining;
            f32 depth;
        };
        std::vector<Particle> particles;

        u32 maxParticles, nextParticle;
        f32 gravity;

};

}   //  namespace

#endif

