#include "Emitter.h"

Emitter::Emitter(EmitterConfig emitter_config_)
	: emitter_config(emitter_config_) {
}

void Emitter::spawnParticle(ParticlePool& pool) {
	int index = pool.acquire();
	if (index != -1) {
		float theta = RandomFloat(0.0f, 2 * PI); // We're using spherical coordinates
		float phi = RandomFloat(0.0f, emitter_config.spread * PI / 180.0f);// Convert spread from degrees to radians
		glm::vec3 offset = glm::vec3(// Convert spherical coordinates to Cartesian coordinates
			sin(phi) * cos(theta),
			sin(phi) * sin(theta),
			cos(phi)
		);
		glm::vec3 randomDir = glm::normalize(emitter_config.direction + offset);// Add the random offset to the emitter's direction and normalize it to get the final direction for the particle
		glm::vec3 vel = randomDir * emitter_config.speed;

		writeParticle(pool.particles[index], vel);
	}
}

void Emitter::writeParticle(Particle& p, glm::vec3 velocity) {
	p.position = glm::vec4(emitter_config.position, emitter_config.size);
	p.velocity = glm::vec4(velocity, 0.0f);
	p.color = emitter_config.startColor;

	p.life = emitter_config.particleLifetime;
	p.maxLife = emitter_config.particleLifetime;
}

void Emitter::setPosition(const glm::vec3& newPosition) {
	emitter_config.position = newPosition;
}

void Emitter::setDirection(const glm::vec3& newDirection) {
	emitter_config.direction = newDirection;
}

void Emitter::setSpeed(float newSpeed) {
	emitter_config.speed = newSpeed;
}

void Emitter::setSpeedVariation(float newSpeedVariation) {
	emitter_config.speedVariation = newSpeedVariation;
}

void Emitter::setSpread(float newSpread) {
	emitter_config.spread = newSpread;;
}

void Emitter::setParticleLifetime(float newLifetime) {
	emitter_config.particleLifetime = newLifetime;
}

void Emitter::setParticleLifetimeVariation(float newParticleLifetimeVariation) {
	emitter_config.particleLifetimeVariation = newParticleLifetimeVariation;
}

void Emitter::setStartColor(const glm::vec4& color) {
	emitter_config.startColor = color;
}

void Emitter::setEndColor(const glm::vec4& color) {
	emitter_config.endColor = color;
}

void Emitter::setSize(float newSize) {
	emitter_config.size = newSize;
}

const EmitterConfig& Emitter::getConfig() const {
	return emitter_config;
}