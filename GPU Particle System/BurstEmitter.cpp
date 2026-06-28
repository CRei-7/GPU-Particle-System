#include<iostream>
#include "BurstEmitter.h"

BurstEmitter::BurstEmitter(EmitterConfig emitter_config_, int count_) :
	Emitter(emitter_config_), count(count_), spawnAccumulator(0.0f), spawnRate(2.5f){ }

void BurstEmitter::update(float deltaTime, ParticlePool& pool) {
    spawnAccumulator += deltaTime;
    while (spawnAccumulator >= 1.0f / spawnRate) {
        spawnParticle(pool);
        spawnAccumulator -= 1.0f / spawnRate;
    }
}

void BurstEmitter::spawnParticle(ParticlePool& pool) {
    int index = pool.acquire();
    if (index == -1) return;

    glm::vec3 vel = glm::normalize(emitter_config.direction) * emitter_config.speed;
    writeParticle(pool.particles[index], vel);
}

void BurstEmitter::writeParticle(Particle& p, glm::vec3 velocity) {
    float x = emitter_config.position.x + RandomFloat(-1.0f, 1.0f);
    p.position = glm::vec4(x, emitter_config.position.y, emitter_config.position.z, emitter_config.size);
    p.velocity = glm::vec4(velocity, 1.0f); //1.0f = burst on death
    p.color = emitter_config.startColor;

    p.life = emitter_config.particleLifetime;
    p.maxLife = emitter_config.particleLifetime;
}

void BurstEmitter::spawnBurst(ParticlePool& pool, glm::vec3 position) {
    for (int i = 0; i < count; ++i)
        spawnChildParticle(pool, position);
}

void BurstEmitter::spawnChildParticle(ParticlePool& pool, glm::vec3 position) {
    int index = pool.acquire();
    if (index == -1) return;

    Particle& p = pool.particles[index];

    float theta = RandomFloat(0.0f, 2.0f * PI);
    float phi = acosf(RandomFloat(-1.0f, 1.0f)); 

    glm::vec3 vel(
        sinf(phi) * cosf(theta),
        sinf(phi) * sinf(theta),
        cosf(phi)
    );
    vel *= emitter_config.speed;

    p.position = glm::vec4(position, emitter_config.size);
    p.velocity = glm::vec4(vel, 0.0f); // 0.0f = no burst on death
    p.color = emitter_config.startColor;
    p.life = emitter_config.particleLifetime * RandomFloat(0.3f, 0.5f); // Children die faster
    p.maxLife = p.life;
}