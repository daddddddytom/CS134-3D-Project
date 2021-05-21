//
//  EntityLander.h
//  CS134-3D-Project
//

#ifndef EntityLander_h
#define EntityLander_h
#include "EntityBase.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "Octree.h"

class EntityLander : public EntityBase {
protected:
	// y axis, AD rotation, WS rotation
	//glm::vec3 heading, horizontalAxis, normalAxis;

	glm::vec3 heading = glm::vec3(0, 1, 0);
	glm::vec3 horizontalAxis = glm::vec3(1, 0, 0);
	glm::vec3 normalAxis = glm::vec3(0, 0, 1);






public:

	// basic logic stuff
	float fuel = 120.0;
	float thrusterStartTime;
	// thruster go brrrr
	ParticleEmitter mainThruster;
	ParticleEmitter explosion;
	ThrustForce thrustforce = ThrustForce(head() * -10);
	bool thrusterOn = false;
	bool XLthrusterOn = false;
	bool XRthrusterOn = false;
	bool ZLthrusterOn = false;
	bool ZRthrusterOn = false;
	bool rotateZACW = false;
	bool rotateZCW = false;
	bool rotateXACW = false;
	bool rotateXCW = false;
	bool rotateYACW = false;
	bool rotateYCW = false;
	bool rotateNormal = false;

	//shader
	ofVbo vbo;


	void loadVbo();


	EntityLander(string fileName);

	glm::vec3 get_heading() const {
		return heading;
	}

	void set_heading(const glm::vec3& heading) {
		this->heading = heading;
	}

	glm::vec3 get_horizontal_axis() const {
		return horizontalAxis;
	}

	void set_horizontal_axis(const glm::vec3& horizontal_axis) {
		horizontalAxis = horizontal_axis;
	}

	glm::vec3 get_normal_axis() const {
		return normalAxis;
	}

	void set_normal_axis(const glm::vec3& normal_axis) {
		normalAxis = normal_axis;
	}

	float get_fuel() const {
		return fuel;
	}

	void set_fuel(float fuel) {
		this->fuel = fuel;
	}



	void update();

	void draw();


	glm::vec3 head() {
		glm::vec3 initialHeading = glm::vec3(0, 1, 0);    // heading at start
		glm::mat4 MrotZ = glm::rotate(glm::mat4(1.0), glm::radians(rotationZ), glm::vec3(0, 0, 1));
		glm::mat4 MrotX = glm::rotate(glm::mat4(1.0), glm::radians(rotationX), glm::vec3(1, 0, 0));
		glm::mat4 MrotY = glm::rotate(glm::mat4(1.0), glm::radians(rotationY), glm::vec3(0, 1, 0));
		glm::vec3 h = MrotZ * glm::vec4(initialHeading, 1);
		h = MrotX * glm::vec4(h, 1);
		h = MrotY * glm::vec4(h, 1);
		return glm::normalize(h);
	}

	glm::vec3 right() {
		glm::vec3 initialHeading = glm::vec3(1, 0, 0);    // horizontal right vector at start
		glm::mat4 MrotZ = glm::rotate(glm::mat4(1.0), glm::radians(rotationZ), glm::vec3(0, 0, 1));
		glm::mat4 MrotX = glm::rotate(glm::mat4(1.0), glm::radians(rotationX), glm::vec3(1, 0, 0));
		glm::mat4 MrotY = glm::rotate(glm::mat4(1.0), glm::radians(rotationY), glm::vec3(0, 1, 0));
		glm::vec3 h = MrotZ * glm::vec4(initialHeading, 1);
		h = MrotX * glm::vec4(h, 1);
		h = MrotY * glm::vec4(h, 1);
		return glm::normalize(h);
	}

	glm::vec3 back() {
		glm::vec3 initialHeading = glm::vec3(0, 0, 1);    // horizontal back vector at start
		glm::mat4 MrotZ = glm::rotate(glm::mat4(1.0), glm::radians(rotationZ), glm::vec3(0, 0, 1));
		glm::mat4 MrotX = glm::rotate(glm::mat4(1.0), glm::radians(rotationX), glm::vec3(1, 0, 0));
		glm::mat4 MrotY = glm::rotate(glm::mat4(1.0), glm::radians(rotationY), glm::vec3(0, 1, 0));
		glm::vec3 h = MrotZ * glm::vec4(initialHeading, 1);
		h = MrotX * glm::vec4(h, 1);
		h = MrotY * glm::vec4(h, 1);
		return glm::normalize(h);
	}

	float getAltitude(Terrain& terrain);

	void explode();
	bool isUpright(glm::vec3 head);
};


#endif /* EntityLander_h */
