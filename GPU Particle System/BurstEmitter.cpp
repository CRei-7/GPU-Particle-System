#include "BurstEmitter.h"

BurstEmitter::BurstEmitter(EmitterConfig emitter_config_, int count_) :
	Emitter(emitter_config_), count(count_), triggered(false) { }

void BurstEmitter::update(float deltaTime, ParticlePool& pool) {
    if (!triggered) return;

    for (int i = 0; i < count; ++i)
        spawnParticle(pool);

    triggered = false;
}

void BurstEmitter::spawnParticle(ParticlePool& pool) {
    int index = pool.acquire();
    if (index == -1) return;

    float theta = RandomFloat(0.0f, 2.0f * PI);
    float phi = RandomFloat(0.0f, 2.0f * PI);

    glm::vec3 vel(
        std::sin(phi) * std::cos(theta),
        std::sin(phi) * std::sin(theta),
        std::cos(phi)
    );
    vel *= emitter_config.speed;

    writeParticle(pool.particles[index], vel);
}