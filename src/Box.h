#pragma once
#ifndef _BOX_H_
#define _BOX_H_

#include "ray.h"
#include "ofMain.h"

/*
 * Axis-aligned bounding box class, for use with the optimized ray-box
 * intersection test described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

class Box {
public:
	Box() {}
	Box(const glm::vec3 &min, const glm::vec3 &max) {
		//     assert(min < max);
		parameters[0] = min;
		parameters[1] = max;
	}
	// (t0, t1) is the interval for valid hits
	bool intersect(const Ray &, float t0, float t1) const;

	// corners
	glm::vec3 parameters[2];

	glm::vec3 min() { return parameters[0]; }
	glm::vec3 max() { return parameters[1]; }

	bool inside(const glm::vec3 &p) const;
	bool inside(const glm::vec3 *points, int size) const;

	bool overlap(const Box &box) const;

	glm::vec3 center() {
		return ((max() - min()) / 2 + min());
	}

	float height() {
		return (max() - min()).y;
	}
	float width() {
		return (max() - min()).x;
	}


	static Box meshBounds(const ofMesh &mesh);
};

#endif // _BOX_H_
