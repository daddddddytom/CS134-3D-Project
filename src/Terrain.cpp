//
//  Terrain.cpp
//  CS134-3D-Project
//
//

#include "Terrain.h"


bool Terrain::intersect(const Ray& ray) {
	TreeNode dummyNode;
	return octree.intersect(ray, octree.root, dummyNode);
}

bool Terrain::overlap(const Box& box) {
	vector<Box> dummy;
	return octree.intersect(box, octree.root, dummy);
}
