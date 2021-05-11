#include "ObjectBase.h"

glm::vec3 ObjectBase::getRotation() {
	return this->rotation;
}

float ObjectBase::getXRotation() {
	return this->rotation.x;
}

float ObjectBase::getYRotation() {
	return this->rotation.y;
}

float ObjectBase::getZRotation() {
	return this->rotation.z;
}

void ObjectBase::setRotation(glm::vec3 rotation) {
	setXRotation(rotation.x);
	setYRotation(rotation.y);
	setZRotation(rotation.z);
}

void ObjectBase::setXRotation(float rotation) {
	float oldRotation = this->rotation.x;
	this->rotation.x = rotation;

	// update the velocity to match
	this->velocity = glm::rotate(glm::mat4(1.0f), glm::radians(rotation - oldRotation), glm::vec3(1, 0, 0)) * this->velocity;
}

void ObjectBase::setYRotation(float rotation) {
	float oldRotation = this->rotation.x;
	this->rotation.x = rotation;

	// update the velocity to match
	this->velocity = glm::rotate(glm::mat4(1.0f), glm::radians(rotation - oldRotation), glm::vec3(0, 1, 0)) * this->velocity;
}

void ObjectBase::setZRotation(float rotation) {
	float oldRotation = this->rotation.x;
	this->rotation.x = rotation;

	// update the velocity to match
	this->velocity = glm::rotate(glm::mat4(1.0f), glm::radians(rotation - oldRotation), glm::vec3(0, 0, 1)) * this->velocity;
}

glm::vec3 ObjectBase::getScale() {
	return this->scale;
}

void ObjectBase::setScale(glm::vec3 scale) {
	this->scale = scale;
}


glm::mat4 ObjectBase::getTMatrix() {
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(this->getPos()));
	glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(this->getXRotation()), glm::vec3(1, 0, 0));
	glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(this->getXRotation()), glm::vec3(0, 1, 0));
	glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(this->getXRotation()), glm::vec3(0, 0, 1));
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), this->getScale());

	//order: right to left
	glm::mat4 t = trans * rotZ * rotY * rotX * scale;

	return t;
}

glm::vec4 ObjectBase::getPos() {
	return this->pos;
}

void ObjectBase::setPos(glm::vec4 pos) {
	this->pos = pos;
}

glm::vec4 ObjectBase::getVelocity() {
	return this->velocity;
}

void ObjectBase::setVelocity(glm::vec4 heading) {
	this->velocity = heading;
}
