/*
 *	CS 134 Final Project
 *  Team: Hugo Wong, Hanqi Dai (Tom), Tomer Erlich
 *
 *  EntityLander.cpp
 */

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

	thrusterSound.load("geo/thruster.wav");
	thrusterSound.setLoop(true);
	thrusterSound.play();
	thrusterSound.setPaused(true);
}

void EntityLander::update() {
	float finalVolume = 0;

	if (isActive) {
		if (thrusterOn) {
			mainThruster.setRate(10);
			this->force += head() * 10;
			finalVolume += 0.5;
		} else {
			mainThruster.setRate(0);
		}
		if (XRthrusterOn) {
			this->force += right() * 10;
		}
		if (XLthrusterOn) {
			this->force -= right() * 10;
		}
		if (ZRthrusterOn) {

			this->force += back() * 10;
		}
		if (ZLthrusterOn) {
			this->force -= back() * 10;
		}


		if (rotateXACW) {
			torqueX += 100;
			finalVolume += 0.05;
		} else if (rotateXCW) {
			torqueX -= 100;
			finalVolume += 0.05;
		} else if (!rotateXCW && !rotateXACW) {
			torqueX = -rotationX / 2;
			finalVolume += abs(rotationX) < 10 ? 0 : 0.05;
		}
		if (rotateYACW) {
			torqueY += 100;
			finalVolume += 0.05;
		} else if (rotateYCW) {
			torqueY -= 100;
			finalVolume += 0.05;
		} else if (!rotateYCW && !rotateYACW) {
			torqueY = -rotationY / 2;
			finalVolume += abs(rotationY) < 10 ? 0 : 0.05;
		}
		if (rotateZACW) {
			torqueZ += 100;
			finalVolume += 0.05;
		} else if (rotateZCW) {
			torqueZ -= 100;
			finalVolume += 0.05;
		} else if (!rotateZCW && !rotateZACW) {
			torqueZ = -rotationZ / 2;
			finalVolume += abs(rotationZ) < 10 ? 0 : 0.05;
		}
	} else {
		mainThruster.setRate(0);
	}

	thrusterSound.setVolume(finalVolume > 1 ? 1 : finalVolume);
	thrusterSound.setPaused(finalVolume < 0.05);

	// thruster sounds

	/*
	if (rotationX >= 20) {
		rotationX = 20;
	}
	if (rotationX <= -20) {
		rotationX = -20;
	}
	if (rotationZ >= 20) {
		rotationZ = 20;
	}
	if (rotationZ <= -20) {
		rotationZ = -20;
	}
	*/
	mainThruster.update();
	mainThruster.setPosition(this->getPosition());

	this->integrate();

	// update the hitbox
	updateHitbox();

}


void EntityLander::updateHitbox() {
	glm::vec3 min = this->getSceneMin();
	glm::vec3 max = this->getSceneMax();
	glm::mat4 MrotZ = glm::rotate(glm::mat4(1.0), glm::radians(rotationZ), glm::vec3(0, 0, 1));
	glm::mat4 MrotX = glm::rotate(glm::mat4(1.0), glm::radians(rotationX), glm::vec3(1, 0, 0));
	glm::mat4 MrotY = glm::rotate(glm::mat4(1.0), glm::radians(rotationY), glm::vec3(0, 1, 0));
	min = MrotY * MrotX * MrotZ * glm::vec4(min, 1) + this->getPosition();
	max = MrotY * MrotX * MrotZ * glm::vec4(max, 1) + this->getPosition();
	if (min.x > max.x) {
		float temp = min.x;
		min.x = max.x;
		max.x = temp;
	}
	if (min.y > max.y) {
		float temp = min.y;
		min.y = max.y;
		max.y = temp;
	}
	if (min.z > max.z) {
		float temp = min.z;
		min.z = max.z;
		max.z = temp;
	}
	hitbox = Box(glm::vec3(min.x, min.y, min.z), glm::vec3(max.x, max.y, max.z));
}


void EntityLander::loadVbo() {
	if (mainThruster.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < mainThruster.sys->particles.size(); i++) {
		points.push_back(mainThruster.sys->particles[i].position);
		sizes.push_back(ofVec3f(mainThruster.particleRadius));
	}



	int total = (int)points.size();
	vbo.clear();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}





void EntityLander::draw() {
	// Draw the lander.
	EntityBase::draw(OF_MESH_FILL);
	ofSetColor(255, 255, 255);
	// Draw the particles
//	this->mainThruster.draw();






	if (mainThruster.started) {
		loadVbo();
		glDepthMask(GL_FALSE);

		// this makes everything look glowy :)
		//
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofEnablePointSprites();

		// begin drawing in the camera

		//theCam->begin();
		vbo.draw(GL_POINTS, 0, (int)mainThruster.sys->particles.size());

		//  end drawing in the camera
		//
		//theCam->end();


		ofDisablePointSprites();
		ofDisableBlendMode();
		ofEnableAlphaBlending();

		// set back the depth mask
		//
		glDepthMask(GL_TRUE);
	}




	//Octree::drawBox(hitbox);
}

float EntityLander::getAltitude(Terrain& terrain) {
	Ray ray = Ray(this->getPosition(), glm::vec3(0, -0.1, 0));
	glm::vec3 intersectionPoint = terrain.intersect(ray);
	return this->getPosition().y - intersectionPoint.y;
}

void EntityLander::explode() {
	cout << "I blew up" << endl;
}

bool EntityLander::isUpright(glm::vec3 head) {
	ofVec3f upright = glm::vec3(0, 1, 0);
	if (upright.angle(head) < 15.0) {
		return true;
	} else
		return false;
}