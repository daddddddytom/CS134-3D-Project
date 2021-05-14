//
//  EntityLander.cpp
//  CS134-3D-Project
//

#include "EntityLander.h"

void EntityLander::setRotation(int which, float angle, float rot_x, float rot_y, float r_z) {
	EntityBase::setRotation(which, angle, rot_x, rot_y, r_z);

	// rotate the 3 axis
}

void EntityLander::draw() {
	// Draw the lander.
	EntityBase::draw(OF_MESH_FILL);

	// Draw the particles
	this->mainThruster.draw();
	
}
