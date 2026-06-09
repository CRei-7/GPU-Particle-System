#include "ContinuousEmitter.h"

ContinuousEmitter::ContinuousEmitter(EmitterConfig emitter_config_, float spawnRate_) : 
	Emitter(emitter_config_), spawnRate(spawnRate_), accumulator(0.0) {
}

void ContinuousEmitter::update(float deltaTime, ParticlePool& pool) {
	accumulator += deltaTime;
	while (accumulator >= 1.0f / spawnRate) {
		spawnParticle(pool);//spawns a single particle using the emitter's properties and adds it to the pool
		accumulator -= 1.0f / spawnRate;
	}
}