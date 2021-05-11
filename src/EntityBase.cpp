#include "EntityBase.h"

EntityBase::EntityBase(string fileName) {
	if (!this->loadModel(fileName, true)) {
		cout << "cannot load " << fileName << endl;
		ofExit(1);
	}
	this->setScaleNormalization(false);
	this->hitbox = Box::meshBounds(this->getMesh(0));
}

EntityBase::EntityBase(const EntityBase& original) {
	// minimum functionality as fail safe, but please avoid using this at all cost.
	cout << "Warning: Copy ctor for EntityBase called. Consider using object reference instead." << endl;
	this->hitbox = original.hitbox;
	this->setPosition(original.pos.x, original.pos.y, original.pos.z);
}

bool EntityBase::inside(const glm::vec3& p) const {
	return hitbox.inside(p);
}

bool EntityBase::inside(const glm::vec3* points, int size) const {
	return hitbox.inside(points, size);
}

bool EntityBase::intersect(const Ray& ray) {
	return hitbox.intersect(ray, 0, std::numeric_limits<float>::infinity());
}

bool EntityBase::overlap(const Box& box) {
	return hitbox.overlap(box);
}

void EntityBase::integrate() {
	// delta time
	float dt = 1.0 / ofGetFrameRate();

	// s(t) = vt
	pos += velocity * dt;

	// F = ma; a = F/m
	ofVec3f accel = (forces / mass);

	// v(t + dt) = v(t) + a(t) * dt 
	velocity += accel * dt;

	// angular accel = torque / I (completely arbitrary)
	glm::vec3 angularAccel = 1000 * torque / mass;

	// update dTheta
	dTheta += angularAccel;

	// angular velocity = change in theta / change in time
	this->setRotation(0, dTheta.x, 1, 0, 0);
	this->setRotation(1, dTheta.y, 0, 1, 0);
	this->setRotation(2, dTheta.z, 0, 0, 1);

	// damping
	velocity *= damping;
	dTheta *= damping;

	// clear forces
	forces = glm::vec3(0, 0, 0);
	torque = glm::vec3(0, 0, 0);
	
}


