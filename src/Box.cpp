#include "box.h"

/*
 * Ray-box intersection using IEEE numerical properties to ensure that the
 * test is both robust and efficient, as described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

 /*
  *	CS 134 Final Project
  *  Team: Hugo Wong, Hanqi Dai (Tom), Tomer Erlich
  *
  *  Box.cpp
  */

bool Box::intersect(const Ray &r, float t0, float t1) const {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (parameters[r.sign[0]].x - r.origin.x) * r.inv_direction.x;
	tmax = (parameters[1 - r.sign[0]].x - r.origin.x) * r.inv_direction.x;
	tymin = (parameters[r.sign[1]].y - r.origin.y) * r.inv_direction.y;
	tymax = (parameters[1 - r.sign[1]].y - r.origin.y) * r.inv_direction.y;
	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	tzmin = (parameters[r.sign[2]].z - r.origin.z) * r.inv_direction.z;
	tzmax = (parameters[1 - r.sign[2]].z - r.origin.z) * r.inv_direction.z;
	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	return ((tmin < t1) && (tmax > t0));
}

bool Box::inside(const glm::vec3& p) const {
	return ((p.x >= parameters[0].x && p.x <= parameters[1].x) &&
		(p.y >= parameters[0].y && p.y <= parameters[1].y) &&
		(p.z >= parameters[0].z && p.z <= parameters[1].z));
}

bool Box::inside(const glm::vec3* points, int size) const {
	bool allInside = true;
	for (int i = 0; i < size; i++) {
		if (!inside(points[i])) allInside = false;
		break;
	}
	return allInside;
}

bool Box::overlap(const Box& box) const {
	glm::vec3 aMin = this->parameters[0];
	glm::vec3 aMax = this->parameters[1];
	glm::vec3 bMin = box.parameters[0];
	glm::vec3 bMax = box.parameters[1];

	return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
		(aMin.y <= bMax.y && aMax.y >= bMin.y) &&
		(aMin.z <= bMax.z && aMax.z >= bMin.z);
}

Box Box::meshBounds(const ofMesh& mesh) {
	int n = mesh.getNumVertices();
	glm::vec3 v = mesh.getVertex(0);
	glm::vec3 max = v;
	glm::vec3 min = v;
	for (int i = 1; i < n; i++) {
		glm::vec3 v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	return Box(glm::vec3(min.x, min.y, min.z), glm::vec3(max.x, max.y, max.z));
}


