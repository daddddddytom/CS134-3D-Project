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
	//this->setPosition(original.pos.x, original.pos.y, original.pos.z);
}


EntityBase::EntityBase() {
	glm::vec3 force = glm::vec3(0, 0, 0);
	glm::vec3 velocity = glm::vec3(0, 0, 0);

	glm::vec3 dTheta = glm::vec3(0, 0, 0);
	glm::vec3 torque = glm::vec3(0, 0, 0);




	float damping = 0.99;
	float mass = 1;
	
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
	float dt = 1.0f / (ofGetFrameRate() <= 1 ? 1 : ofGetFrameRate());
	// s(t) = vt
	pos += velocity * dt;

	// F = ma; a = F/m
	glm::vec3 accel = (force / mass);

	// v(t + dt) = v(t) + a(t) * dt
	velocity += accel * dt;

	// angular accel = torque / I (completely arbitrary)
	glm::vec3 angularAccel = torque / mass;
	

	// update dTheta
	dTheta += angularAccel;

	// angular velocity = change in theta / change in time
	// not all of these can run together or only the last on will be called
	this->setRotation(0, rotationX , 1, 0, 0);
	this->setRotation(1, rotationY , 0, 1, 0);
	this->setRotation(2, rotationZ , 0, 0, 1);
	
	angularAccelerationZ = torqueZ / mass;
	angularAccelerationX = torqueX / mass;
	angularAccelerationY = torqueY / mass;
	angularVelocityZ += angularAccelerationZ * dt;
	angularVelocityZ *= damping;
	angularVelocityX += angularAccelerationX * dt;
	angularVelocityX *= damping;
	angularVelocityY += angularAccelerationY * dt;
	angularVelocityY *= damping;
	rotationZ += (angularVelocityZ * dt);
	rotationX += (angularVelocityX * dt);
	rotationY += (angularVelocityY * dt);


	// damping
	velocity *= damping;
	dTheta *= damping;

	// clear forces
	force = glm::vec3(0, 0, 0);
	torque = glm::vec3(0, 0, 0);
	torqueZ = 0;
	torqueX = 0;
	torqueY = 0;
}

void EntityBase::addForce(glm::vec3 quantity) {
	this->force += quantity;
}

void EntityBase::addTorque(glm::vec3 quantity) {
	this->torque += quantity;
}



