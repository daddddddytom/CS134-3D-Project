#include "ofMain.h"

class ObjectBase {
protected:
	glm::vec4 pos;
	glm::vec4 velocity;
	// rotation about the x, y, and z axis
	glm::vec3 rotation;
	glm::vec3 scale;

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

	glm::vec4 getPos();
	void setPos(glm::vec4 pos);
	glm::vec4 getVelocity();
	void setVelocity(glm::vec4 heading);

	glm::vec3 getRotation();
	float getXRotation();
	float getYRotation();
	float getZRotation();
	virtual void setRotation(glm::vec3 rotation);
	virtual void setXRotation(float rotation);
	virtual void setYRotation(float rotation);
	virtual void setZRotation(float rotation);
	

	glm::vec3 getScale();
	void setScale(glm::vec3 scale);

	// Return the transformation matrix of this given shape.
	glm::mat4 getTMatrix();

};