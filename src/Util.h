#pragma once
//  Kevin M. Smith - CS 134 SJSU

#include "ofMain.h"

bool rayIntersectPlane(const glm::vec3 &rayPoint, const glm::vec3 &raydir, glm::vec3 const &planePoint,
	const glm::vec3 &planeNorm, glm::vec3 &point);

glm::vec3 reflectVector(const glm::vec3 &v, const glm::vec3 &normal);



