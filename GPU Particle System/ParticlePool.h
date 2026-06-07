#pragma once
#include <vector>
#include "Particle.h"

class ParticlePool
{
public:
	ParticlePool();
	ParticlePool(int maxParticles_);
	
	ParticlePool(const ParticlePool&) = delete;// Disable copy constructor
	ParticlePool& operator=(const ParticlePool&) = delete;// Disable copy assignment operator

	ParticlePool(ParticlePool&&) = default;// Enable move constructor
	ParticlePool& operator=(ParticlePool&&) = default;// Enable move assignment operator

	int acquire();

	void release(int index);

	void releaseDeadParticles();

	int AliveCount() const;
	int capacity() const;
	bool isFull() const;

	std::vector<Particle> particles;

	~ParticlePool();

private:
	std::vector<int> freeList;
	int freeCount;
	int maxParticles;
};

