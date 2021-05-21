#include "Terrain.h"

/*
  *	CS 134 Final Project
  *  Team: Hugo Wong, Hanqi Dai (Tom), Tomer Erlich
  *
  *  Terrain.cpp
  */
glm::vec3 Terrain::intersect(const Ray& ray) {
	TreeNode dummyNode;
	if (octree.intersect(ray, octree.root, dummyNode)) {
		return glm::vec3(dummyNode.box.center().x, dummyNode.box.max().y, dummyNode.box.center().z);
	}
	return glm::vec3(0, 0, 0);
}

bool Terrain::overlap(const Box& box) {
	vector<Box> dummy;
	return octree.intersect(box, octree.root, dummy);
}
