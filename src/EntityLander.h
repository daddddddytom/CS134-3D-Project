//
//  EntityLander.h
//  CS134-3D-Project
//

#ifndef EntityLander_h
#define EntityLander_h
#include "EntityBase.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"

class EntityLander : public EntityBase {
protected:
	// y axis, AD rotation, WS rotation
	glm::vec3 heading, horizontalAxis, normalAxis;

	// basic logic stuff
	float fuel;

	// thruster go brrrr
	ParticleEmitter mainThruster;
	ThrustForce thrustforce = ThrustForce(glm::vec3(0, -20, 0));

public:
	bool thrusterOn = false;

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

};


#endif /* EntityLander_h */
