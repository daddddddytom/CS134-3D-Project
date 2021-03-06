#pragma once
/*
 *	CS 134 Final Project
 *  Team: Hugo Wong, Hanqi Dai (Tom), Tomer Erlich
 *
 *  ParticleEmitter.h
 *
 *  Base code given by Prof. Smith
 */
#include "TransformObject.h"
#include "ParticleSystem.h"

typedef enum { DirectionalEmitter, RadialEmitter, SphereEmitter, DiscEmitter } EmitterType;

//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class ParticleEmitter : public TransformObject {
public:
	ParticleEmitter();
	ParticleEmitter(ParticleSystem *s);
	~ParticleEmitter();
	void init();
	void draw();
	void start();
	void stop();
	void setDamping(const float damp);
	void setLifespan(const float life)   { lifespan = life; }
	void setVelocity(const ofVec3f &vel) { velocity = vel; }
	void setRate(const float r) { rate = r; }
	void setParticleRadius(const float r) { particleRadius = r; }
	void setEmitterType(EmitterType t) { type = t; }
	void setGroupSize(int s) { groupSize = s; }
	void setOneShot(bool s) { oneShot = s; }
	void update();
	void spawn(float time);
	ParticleSystem *sys;
	float rate;         // per sec
	bool oneShot;
	bool fired;
	ofVec3f velocity;
	float lifespan;     // sec
	bool started;
	float lastSpawned;  // ms
	float particleRadius;
	float radius;
	bool visible;
	float damping = 0.99;
	int groupSize;      // number of particles to spawn in a group
	bool createdSys;
	EmitterType type;
	ofColor color;
	void ParticleEmitter::setColor(const ofColor &color);
};
