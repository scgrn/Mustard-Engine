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

#include "../pch.h"

#include "particleSystem.h"
#include "sprite.h"

namespace AB {

ParticleSystem::ParticleSystem(u32 maxParticles) {
    particles.resize(maxParticles);
    this->maxParticles = maxParticles;
    this->gravity = gravity;
    nextParticle = 0;
    active = false;
}

void ParticleSystem::emit(ParticleParameters const& parameters) {
    Particle& particle = particles[nextParticle];

    particle.pos = parameters.pos;
    particle.vel = parameters.vel;
    particle.acceleration = parameters.acceleration;
    particle.dampening = parameters.dampening;

    particle.startColor = parameters.startColor;
    particle.endColor = parameters.endColor;
    particle.startScale = parameters.startScale;
    particle.endScale = parameters.endScale;
    particle.angle = rndf(parameters.minAngle, parameters.maxAngle);
    particle.rotation = rndf(parameters.minRotation, parameters.maxRotation);

    particle.startSprite = parameters.startSprite;
    particle.endSprite = parameters.endSprite;

    particle.lifetime = rnd(parameters.minLifetime, parameters.maxLifetime);
    particle.lifetimeRemaining = particle.lifetime;

    nextParticle = (nextParticle + 1) % maxParticles;

    active = true;
}

void ParticleSystem::update() {
    active = false;
    for (auto& particle : particles) {
        if (particle.lifetimeRemaining <= 0) {
            continue;
        }

        active = true;

        particle.pos += particle.vel;
        particle.vel += particle.acceleration;
        particle.vel *= particle.dampening;
        particle.angle += particle.rotation;
 
        particle.lifetimeRemaining--;
    }
}

void ParticleSystem::render(RenderLayer *renderLayer, PerspectiveCamera const& camera) {
    //  calculate viewspace depth for each particle and sort
    for (auto& particle : particles) {
        Mat4 view = camera.viewMatrix;
        particle.depth = (view * particle.pos).z;
    }
    std::sort(particles.begin(), particles.end(),
        [](const Particle& a, const Particle& b) {
            return a.depth < b.depth;
        });

    for (auto& particle : particles) {
        if (particle.lifetimeRemaining <= 0) {
            continue;
        }

        f32 life = 1.0f - ((f32)particle.lifetimeRemaining / (f32)particle.lifetime);

        extern AssetManager<Sprite> sprites;
        u32 spriteIndex = (u32)((particle.endSprite - particle.startSprite) * life) + particle.startSprite;
        Sprite* sprite = sprites.get(spriteIndex);

        f32 scale = ((particle.endScale - particle.startScale) * life) + particle.startScale;
        f32 hScale = (1.0f / sprite->width) * scale;
        f32 vScale = (1.0f / sprite->height) * scale;

        Vec4 color = ((particle.endColor - particle.startColor) * life) + particle.startColor;

        sprite->render(renderLayer, particle.pos, particle.angle, AB::Vec2(hScale, vScale), color);
    }
}

}    //  namespace

