#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "Box.h"

/*
What ofxAssimpModelLoader has:
- position, rotation, scale
- wireframe drawing, face drawing, axis drawing

What ofxAssimpModelLoader doesn't have:
- velocity, angular velocity
- force, torque
- collision checks
*/
class EntityBase : public ofxAssimpModelLoader {
protected:
	Box hitbox;
	
	glm::vec3 force = glm::vec3(0, 0, 0);
	glm::vec3 velocity = glm::vec3(0, 0, 0);

	glm::vec3 dTheta = glm::vec3(0, 0, 0);
	glm::vec3 torque = glm::vec3(0, 0, 0);

	float damping = 0.99;
	float mass = 1;

public:
	EntityBase(string fileName);

	//copy ctor just in case
	EntityBase(const EntityBase &original);
	EntityBase();
	// overridable collision check functions
	virtual bool inside(const glm::vec3 &p) const;
	virtual bool inside(const glm::vec3 *points, int size) const;
	virtual bool intersect(const Ray &ray);
	virtual bool overlap(const Box &box);

	Box getHitbox() {
		return hitbox;
	}
	
	// physics
	virtual void integrate();
	
	void addForce(glm::vec3 quantity);
	void addTorque(glm::vec3 quantity);

	

};