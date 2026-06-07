#include "ParticlePool.h"
#include <cassert>
#include <stdexcept>

ParticlePool::ParticlePool() : particles(500), freeList(500), maxParticles(500), freeCount(500){
	if(maxParticles <= 0) {
		throw std::invalid_argument("maxParticles must be greater than 0");
	}
	
	for (unsigned int i = 0; i < maxParticles; ++i) {
		freeList[i] = maxParticles - 1 - i;
	}
}

ParticlePool::ParticlePool(int maxParticles_) : particles(maxParticles_), freeList(maxParticles_), maxParticles(maxParticles_), freeCount(maxParticles_){
	if(maxParticles <= 0) {
		throw std::invalid_argument("maxParticles must be greater than 0");
	}
	
	for (unsigned int i = 0; i < maxParticles; ++i) {
		freeList[i] = maxParticles - 1 - i;
	}
}

int ParticlePool::acquire() {
	if (freeCount == 0) {
		return -1; // No free particles available
	}

	int index = freeList[--freeCount];

	particles[index].life = 0.0f; //0.0 means the particle is dead, and it will be initialized later when it's used
	particles[index].maxLife = 0.0f;

	return index;
}

void ParticlePool::release(int index) {
	assert(index >= 0 && index < maxParticles);
	particles[index] = Particle{}; // Reset the particle to default state
	freeList[freeCount++] = index;
}

void ParticlePool::releaseDeadParticles() {
	for (unsigned int i = 0; i < maxParticles; ++i) {
		if (particles[i].maxLife > 0.0f && particles[i].life <= 0.0f) {
			release(i);
		}
	}
}

int ParticlePool::AliveCount() const {
	return maxParticles - freeCount;
}

int ParticlePool::capacity() const {
	return maxParticles;
}

bool ParticlePool::isFull() const {
	return freeCount == 0;
}

ParticlePool::~ParticlePool() {
}