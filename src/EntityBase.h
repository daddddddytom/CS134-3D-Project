#include "ofMain.h"

class EntityBase {
protected:
	glm::vec4 pos;
	glm::vec4 velocity;
	float rotation = 0.0f;
	glm::vec4 scale;

public:
	ObjectBase() {}

	ObjectBase(glm::vec4 pos) {
		this->pos = pos;
	}

	ObjectBase(glm::vec4 pos, float rotation, glm::vec4 scale) {
		this->pos = pos;
		this->rotation = rotation;
		this->scale = scale;
	}

	glm::vec3 getPos();
	void setPos(glm::vec4 pos);
	glm::vec3 getVelocity();
	void setVelocity(glm::vec4 heading);

	float getRotation();
	virtual void setRotation(float rotation);

	glm::vec3 getScale();
	void setScale(glm::vec4 scale);

	// Return the transformation matrix of this given shape.
	glm::mat4 getTMatrix();

};