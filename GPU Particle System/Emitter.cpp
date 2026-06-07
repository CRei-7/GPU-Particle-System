#include "Emitter.h"

const float PI = 3.14159265f;

static float RandomFloat(float min, float max) {
	float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX); // Generates a random float between 0.0 and 1.0
	return min + random * (max - min);
}

Emitter::Emitter(glm::vec3 position_, glm::vec3 direction_, float spawnRate_, float speed_, float minSpread_, float maxSpread_, float particleLifetime)
	: position(position_), direction(direction_), spawnRate(spawnRate_), speed(speed_), minSpread(minSpread_), maxSpread(maxSpread_), particleLifetime(particleLifetime), accumulator(0.0f) {
}

void Emitter::spawnParticle(ParticlePool& pool) {
	int index = pool.acquire();
	if (index != -1) {
		Particle& particle = pool.particles[index];
		particle.pos = position;
		
		float theta = RandomFloat(0.0f, 2 * PI); // We're using spherical coordinates
		float phi = RandomFloat(minSpread * PI/180.0f, maxSpread * PI / 180.0f);// Convert spread from degrees to radians
		glm::vec3 offset = glm::vec3(// Convert spherical coordinates to Cartesian coordinates
			sin(phi) * cos(theta),
			sin(phi) * sin(theta),
			cos(phi)
		);
		glm::vec3 randomDir = glm::normalize(direction + offset);// Add the random offset to the emitter's direction and normalize it to get the final direction for the particle
		particle.speed = randomDir * speed;

		particle.r = 255; // Set color to white
		particle.g = 255;
		particle.b = 255;
		particle.a = 255;

		particle.life = particleLifetime;
		particle.maxLife = particleLifetime;
	}
}

void Emitter::update(float deltaTime, ParticlePool &pool) {
	accumulator += deltaTime;
	while (accumulator >= 1.0f / spawnRate) {
		spawnParticle(pool);//spawns a single particle using the emitter's properties and adds it to the pool
		accumulator -= 1.0f / spawnRate;
	}
}

void Emitter::setPosition(const glm::vec3& newPosition) {
	position = newPosition;
}

void Emitter::setDirection(const glm::vec3& newDirection) {
	direction = newDirection;
}

void Emitter::setSpawnRate(float newSpawnRate) {
	spawnRate = newSpawnRate;
}

void Emitter::setSpeed(float newSpeed) {
	speed = newSpeed;
}

void Emitter::setSpread(float newMinSpread, float newMaxSpread) {
	minSpread = newMinSpread;
	maxSpread = newMaxSpread;
}

void Emitter::setParticleLifetime(float newLifetime) {
	particleLifetime = newLifetime;
}

Emitter::~Emitter() {
}
