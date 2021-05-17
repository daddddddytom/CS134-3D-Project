//
//  Terrain.cpp
//  CS134-3D-Project
//
//

#include "Terrain.h"


glm::vec3 Terrain::intersect(const Ray& ray) {
	TreeNode dummyNode;
	octree.intersect(ray, octree.root, dummyNode);
	return glm::vec3(dummyNode.box.center().x, dummyNode.box.max().y, dummyNode.box.center().z);
}

bool Terrain::overlap(const Box& box) {
	vector<Box> dummy;
	return octree.intersect(box, octree.root, dummy);
}
