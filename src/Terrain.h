//
//  EntityTerrain.hpp
//  CS134-3D-Project
//
//  Created by Tomer Erlich on 5/12/21.
//

#ifndef EntityTerrain_hpp
#define EntityTerrain_hpp

#include "ofxAssimpModelLoader.h"
#include "Octree.h"

class Terrain : public ofxAssimpModelLoader {
protected:
	Octree octree;

public:
	bool intersect(const Ray &ray);
	bool overlap(const Box &box);
};



#endif /* EntityTerrain_hpp */
