#pragma once

#include "ofMain.h"
#include "EntityBase.h"

/*
 *	CS 134 Final Project
 *  Team: Hugo Wong, Hanqi Dai (Tom), Tomer Erlich
 *
 *  PhysicsUtil.h
 *
 *  Base code given by Prof. Smith.
 */
class Force {
protected:
public:
	bool applyOnce = false;
	bool applied = false;
	virtual void updateForce(EntityBase* entity) = 0;
};

class GravityForce: public Force {
public:
	glm::vec3 gravity;
	GravityForce(const glm::vec3 & gravity);
	void updateForce(EntityBase* entity);
};

class TurbulenceForce : public Force {
public:
	glm::vec3 tmin, tmax;
	TurbulenceForce(const glm::vec3 & min, const glm::vec3 &max);
	void updateForce(EntityBase* entity);
};