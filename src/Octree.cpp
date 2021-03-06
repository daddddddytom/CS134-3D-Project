
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Simple Octree Implementation 11/10/2020
// 
//  Copyright (c) by Kevin M. Smith
//  Copying or use without permission is prohibited by law. 
//

/*
 *	CS 134 Final Project
 *  Team: Hugo Wong, Hanqi Dai (Tom), Tomer Erlich
 *
 *	Base code given by Prof. Smith
 *	
 *  Octree.cpp
 */
#include "Octree.h"

//draw a box from a "Box" class  
//
void Octree::drawBox(const Box &box) {
	glm::vec3 min = box.parameters[0];
	glm::vec3 max = box.parameters[1];
	glm::vec3 size = max - min;
	glm::vec3 center = size / 2 + min;
	glm::vec3 p = glm::vec3(center.x, center.y, center.z);
	float w = size.x;
	float h = size.y;
	float d = size.z;
	ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//
Box Octree::meshBounds(const ofMesh & mesh) {
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
	cout << "vertices: " << n << endl;
	//	cout << "min: " << min << "max: " << max << endl;
	return Box(glm::vec3(min.x, min.y, min.z), glm::vec3(max.x, max.y, max.z));
}

// getMeshPointsInBox:  return an array of indices to points in mesh that are contained 
//                      inside the Box.  Return count of points found;
//
int Octree::getMeshPointsInBox(const ofMesh & mesh, const vector<int>& points,
	Box & box, vector<int> & pointsRtn) {
	int count = 0;
	for (int i = 0; i < points.size(); i++) {
		glm::vec3 v = mesh.getVertex(points[i]);
		if (box.inside(glm::vec3(v.x, v.y, v.z))) {
			count++;
			pointsRtn.push_back(points[i]);
		}
	}
	return count;
}

// getMeshFacesInBox:  return an array of indices to Faces in mesh that are contained 
//                      inside the Box.  Return count of faces found;
//
int Octree::getMeshFacesInBox(const ofMesh & mesh, const vector<int>& faces,
	Box & box, vector<int> & facesRtn) {
	int count = 0;
	for (int i = 0; i < faces.size(); i++) {
		ofMeshFace face = mesh.getFace(faces[i]);
		glm::vec3 v[3];
		v[0] = face.getVertex(0);
		v[1] = face.getVertex(1);
		v[2] = face.getVertex(2);
		glm::vec3 p[3];
		p[0] = glm::vec3(v[0].x, v[0].y, v[0].z);
		p[1] = glm::vec3(v[1].x, v[1].y, v[1].z);
		p[2] = glm::vec3(v[2].x, v[2].y, v[2].z);
		if (box.inside(p, 3)) {
			count++;
			facesRtn.push_back(faces[i]);
		}
	}
	return count;
}

//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void Octree::subDivideBox8(const Box &box, vector<Box> & boxList) {
	glm::vec3 min = box.parameters[0];
	glm::vec3 max = box.parameters[1];
	glm::vec3 size = max - min;
	glm::vec3 center = size / 2 + min;
	float xdist = (max.x - min.x) / 2;
	float ydist = (max.y - min.y) / 2;
	float zdist = (max.z - min.z) / 2;
	glm::vec3 h = glm::vec3(0, ydist, 0);

	//  generate ground floor
	//
	Box b[8];
	b[0] = Box(min, center);
	b[1] = Box(b[0].min() + glm::vec3(xdist, 0, 0), b[0].max() + glm::vec3(xdist, 0, 0));
	b[2] = Box(b[1].min() + glm::vec3(0, 0, zdist), b[1].max() + glm::vec3(0, 0, zdist));
	b[3] = Box(b[2].min() + glm::vec3(-xdist, 0, 0), b[2].max() + glm::vec3(-xdist, 0, 0));

	boxList.clear();
	for (int i = 0; i < 4; i++)
		boxList.push_back(b[i]);

	// generate second story
	//
	for (int i = 4; i < 8; i++) {
		b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
		boxList.push_back(b[i]);
	}
}

void Octree::updateLeafNodes(int numLevels) {
	leaves.clear();
	updateLeafNodes(root, numLevels, 0);
}

void Octree::updateLeafNodes(TreeNode& node, int numLevels, int level) {
	if (level >= numLevels - 1) {
		leaves.push_back(&node);
		return;
	}
	for (int i = 0; i < node.children.size(); i++) {
		updateLeafNodes(node.children[i], numLevels, level + 1);
	}
}

void Octree::create(const ofMesh & geo, int numLevels) {
	// initialize octree structure
	//
	mesh = geo;
	int level = 0;
	root.box = meshBounds(mesh);
	if (!bUseFaces) {
		for (int i = 0; i < mesh.getNumVertices(); i++) {
			root.points.push_back(i);
		}
	} else {
		// need to load face vertices here
		//
	}

	// recursively buid octree
	//
	level++;
	subdivide(mesh, root, numLevels, level);

	updateLeafNodes(numLevels);
}


void Octree::subdivide(const ofMesh & mesh, TreeNode & node, int numLevels, int level) {
	if (level >= numLevels) return;
	vector<Box> boxList;
	subDivideBox8(node.box, boxList);
	level++;
	int pointsInNode = node.points.size();
	int totalPoints = 0;
	for (int i = 0; i < boxList.size(); i++) {
		TreeNode child;
		int count = 0;
		if (!bUseFaces)
			count = getMeshPointsInBox(mesh, node.points, boxList[i], child.points);
		else
			count = getMeshFacesInBox(mesh, node.points, boxList[i], child.points);
		totalPoints += count;

		if (count > 0) {
			child.box = boxList[i];
			node.children.push_back(child);
			if (count > 1) {
				subdivide(mesh, node.children.back(), numLevels, level);
			}
		}


	}
	// debug
	//
	if (pointsInNode != totalPoints) {
		strayVerts += (pointsInNode - totalPoints);
	}
}

// Implement functions below for Homework project
//

bool Octree::intersect(const Ray &ray, const TreeNode & node, TreeNode & nodeRtn) {
	if (node.box.intersect(ray, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity())) {
		if (node.children.empty()) {
			nodeRtn = node;
			return true;
		}

		for (const TreeNode& child : node.children) {
			if (intersect(ray, child, nodeRtn)) {
				return true;
			}
		}
	}
	return false;
}

/*
// termination at selected level
for (TreeNode* np : leaves) {
	if (np->box.intersect(ray, 0, std::numeric_limits<float>::infinity())) {
		nodeRtn = *np;
		return true;
	}
}
*/


bool Octree::intersect(const ofVec3f &point, TreeNode & node, TreeNode & nodeRtn) {
	glm::vec3 point1 = glm::vec3(point.x, point.y, point.z);

	if (node.box.inside(point1)) {

		if (node.children.size() == 0) {
			nodeRtn = node;
			return true;
		}

		for (int i = 0; i < node.children.size(); i++) {
			intersect(point, node.children[i], nodeRtn);
		}
	} else {
		return false;
	}
}

bool Octree::intersect(const Box &box, TreeNode & node, vector<Box> & boxListRtn) {
	if (node.box.overlap(box)) {
		if (node.children.empty()) {
			boxListRtn.push_back(node.box);
		} else {
			for (int i = 0; i < node.children.size(); i++) {
				intersect(box, node.children[i], boxListRtn);
			}
		}

	}
	return !boxListRtn.empty();
	/*
	boxListRtn.clear();

	for (TreeNode* np : leaves) {
		if (np->box.overlap(box)) {
			boxListRtn.push_back(np->box);
		}
	}

	return !boxListRtn.empty();
	*/
}





void Octree::draw(TreeNode & node, int numLevels, int level) {
	if (level >= numLevels) return;
	drawBox(node.box);
	level++;
	for (int i = 0; i < node.children.size(); i++) {
		draw(node.children[i], numLevels, level);
	}
}

void Octree::drawLeafNodes() {
	for (TreeNode* np : this->leaves) {
		drawBox(np->box);
	}
}




