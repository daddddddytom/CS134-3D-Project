//
//  EntityLander.cpp
//  CS134-3D-Project
//

#include "EntityLander.h"

EntityLander::EntityLander(string fileName) {
	this->loadModel(fileName);
	this->setScaleNormalization(false);
	
	mainThruster.sys->addForce(&thrustforce);
	mainThruster.setEmitterType(DiscEmitter);
	mainThruster.setPosition(this->getPosition());
	mainThruster.setLifespan(1);
	mainThruster.setGroupSize(20);
	mainThruster.setParticleRadius(.02);
	mainThruster.radius = .5;
	mainThruster.setVelocity(ofVec3f(0, 0, 0));
	mainThruster.start();
}

void EntityLander::update() {
	if (thrusterOn) {
		mainThruster.setRate(10);
		this->force += head() * 10;
	} 
	else {
		mainThruster.setRate(0);
	}

	if (rotateZACW) {
		torqueZ += 100;
		
	}
	else if(rotateZACW) {
		torqueZ = 0.0f;
		
	}

	if (rotateZCW) {
		torqueZ -= 100;

	}
	else {
		torqueZ = 0.0f;

	}

	if (rotateXACW) {
		torqueX += 100;

	}
	else {
		torqueX = 0.0f;

	}

	if (rotateXCW) {
		torqueX += 100;

	}
	else {
		torqueX = 0.0f;

	}

	mainThruster.update();
	mainThruster.setPosition(this->getPosition());

	/*
	glm::mat4 headingRotate = glm::rotate(glm::mat4(1), dTheta.x, glm::vec3(1, 0, 0))
		* glm::rotate(glm::mat4(1), dTheta.y, glm::vec3(0, 1, 0))
		* glm::rotate(glm::mat4(1), dTheta.z, glm::vec3(0, 0, 1));
	heading += (this->getPosition() - this->heading);
	heading += velocity / ofGetFrameRate();
	*/
	this->integrate();

	/*
	heading = headingRotate * glm::vec4(heading, 1);
	horizontalAxis = glm::vec3(pos.x + 1, pos.y, pos.z);
	normalAxis = glm::vec3(pos.x, pos.y, pos.z + 1);
	*/
}

void EntityLander::draw() {
	// Draw the lander.
	EntityBase::draw(OF_MESH_FILL);

	// Draw the particles
	this->mainThruster.draw();
}


