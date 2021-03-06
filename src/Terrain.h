/*
  *	CS 134 Final Project
  *  Team: Hugo Wong, Hanqi Dai (Tom), Tomer Erlich
  *
  *  Terrain.h
  */

#ifndef EntityTerrain_hpp
#define EntityTerrain_hpp

#include <glm/detail/_noise.hpp>
#include <glm/detail/_noise.hpp>

#include "ofxAssimpModelLoader.h"
#include "Octree.h"

class Terrain : public ofxAssimpModelLoader {
protected:
	

public:
	Octree octree;
	Terrain(string filename, int hitboxLevel) {
		loadModel(filename);
		setScaleNormalization(false);
		octree.create(this->getMesh(0), hitboxLevel);
	}

	glm::vec3 intersect(const Ray& ray);
	bool overlap(const Box &box);
};



#endif /* EntityTerrain_hpp */
