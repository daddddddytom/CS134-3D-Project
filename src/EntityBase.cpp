#include "EntityBase.h"

float EntityBase::getRotation() {
	return this->rotation;
}

void EntityBase::setRotation(float rotation) {
	float oldRotation = this->rotation;
	this->rotation = rotation;
	this->velocity = glm::rotate(glm::mat4(1.0f), glm::radians(rotation - oldRotation), glm::vec4(0, 0, 1)) * glm::vec4(this->velocity, 0);
}

glm::vec4 EntityBase::getScale() {
	return this->scale;
}

void EntityBase::setScale(glm::vec4 scale) {
	this->scale = scale;
}


glm::mat4 EntityBase::getTMatrix() {
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), this->getPos());
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(this->getRotation()), glm::vec4(0, 0, 1));
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), this->getScale());

	//order: right to left
	glm::mat4 t = trans * rot * scale;

	return t;
}

glm::vec4 EntityBase::getPos() {
	return this->pos;
}

void EntityBase::setPos(glm::vec4 pos) {
	this->pos = pos;
}

glm::vec4 EntityBase::getVelocity() {
	return this->velocity;
}

void EntityBase::setVelocity(glm::vec4 heading) {
	this->velocity = heading;
}
