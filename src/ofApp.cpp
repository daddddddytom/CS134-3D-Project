#include "ofApp.h"
#include "Util.h"
#include "Octree.h"
#include <glm/gtx/intersect.hpp>

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup() {
	gravityForce = new GravityForce(ofVec3f(0, 10, 0));

	lander.setPosition(200, 300, 200);

	pathBox = Box(glm::vec3(0, 0, 0), glm::vec3(200, 300, 200));

	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
	//	ofSetWindowShape(1024, 768);
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();
	cam.setPosition(200, 400, 200);

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();


	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));


}

//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
	checkDistToPath();
	if (bLanderOut) {
		cout << "Mission failed: Lander too far way from path" << endl;
	}


	// check input
	if (inputHandler.getInputState(InputHandler::SPACE)) {
		lander.thrusterOn = true;
	} else {
		lander.thrusterOn = false;
	}

	if (inputHandler.getInputState(InputHandler::A)) {
		//lander.addTorque(glm::vec3(0, 0, -0.1));
		lander.rotateZACW = true;
	}
	else  {

		lander.rotateZACW = false;
		//cout << lander.rotateHeading << endl;
	}
	if (inputHandler.getInputState(InputHandler::D)) {
		//lander.addTorque(glm::vec3(0, 0, -0.1));
		lander.rotateZCW = true;
	}
	else {

		lander.rotateZCW = false;
		//cout << lander.rotateHeading << endl;
	}
	

	if (inputHandler.getInputState(InputHandler::W)) {
		//lander.addTorque(glm::vec3(0.1, 0, 0));
		lander.rotateXACW = true;
	}
	else {
		lander.rotateXACW = false;
	}

	if (inputHandler.getInputState(InputHandler::S)) {
		//lander.addTorque(glm::vec3(-0.1, 0, 0));
		lander.rotateXCW = true;
	}
	else {
		lander.rotateXCW = false;
	}

	collisionDetection();
	lander.update();
	land();


}
//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(ofColor::black);

	glDepthMask(false);
	gui.draw();
	string str1 = "torqueZ: " + std::to_string(lander.torqueZ);
	string str2 = "torqueX: " + std::to_string(lander.torqueX);
	string str3 = "torqueY: " + std::to_string(lander.torqueY);
	ofDrawBitmapString(str1, ofGetWindowWidth() -200, 15);
	ofDrawBitmapString(str2, ofGetWindowWidth() -200, 30);
	ofDrawBitmapString(str3, ofGetWindowWidth() -200, 45);
	glDepthMask(true);
	ofSetColor(255, 255, 255);
	cam.begin();
	ofPushMatrix();
	drawPath(lander.getHitbox());

	ofEnableLighting();              // shaded mode
	terrain.drawFaces();
	ofMesh mesh;
	lander.draw();
	if (!bTerrainSelected) drawAxis(lander.getPosition());
	if (bDisplayBBoxes) {
		ofNoFill();
		ofSetColor(ofColor::white);
		for (int i = 0; i < lander.getNumMeshes(); i++) {
			ofPushMatrix();
			ofMultMatrix(lander.getModelMatrix());
			ofRotate(-90, 1, 0, 0);
			Octree::drawBox(bboxList[i]);
			ofPopMatrix();
		}
	}

	if (bLanderSelected) {

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(glm::vec3(min.x, min.y, min.z), glm::vec3(max.x, max.y, max.z));
		ofNoFill();
		//ofSetColor(ofColor::white);
		Octree::drawBox(bounds);

		// draw colliding boxes
		//
		ofSetColor(ofColor::red);
		for (int i = 0; i < colBoxList.size(); i++) {
			Octree::drawBox(colBoxList[i]);
		}
	}
	
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));

	if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		terrain.octree.draw(numLevels, 0);
	}


	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();


	ofPopMatrix();
	cam.end();
}

// 

// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}

void ofApp::checkDistToPath() {
	glm::vec3 boxCenter = glm::vec3(pathBox.center().x, pathBox.center().y, pathBox.center().z);
	glm::vec3 landerPos = glm::vec3(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z);
	float dist = glm::distance(boxCenter, landerPos);
	//cout << dist << endl;
	if (dist > 500) {
		bLanderOut = true;
	} else {
		bLanderOut = false;
	}
}




void ofApp::drawPath(Box landerBox) {
	ofPushMatrix();

	//ofSetColor(0x000000);
	ofNoFill();


	ofDrawBezier(0, 0, 0, 10, 450, 100, 200, 150, 100, 200, 300, 200);


	//ofDrawLine(ofPoint(400, 600, 0), ofPoint(landerBox.center().x, landerBox.center().y, landerBox.center().z));




	ofPopMatrix();
}



void ofApp::keyPressed(int key) {
	cout << ofGetKeyPressed() << endl;
	switch (key) {
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'r':
		cam.reset();
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_F1:
		theCam = &cam;
		break;
	case OF_KEY_F2:
		theCam = &top;
		break;
	case OF_KEY_UP:
		inputHandler.setInputState(InputHandler::UP, true);
		break;
	case OF_KEY_DOWN:
		inputHandler.setInputState(InputHandler::DOWN, true);
		break;
	case OF_KEY_LEFT:
		inputHandler.setInputState(InputHandler::LEFT, true);
		break;

	case OF_KEY_RIGHT:
		inputHandler.setInputState(InputHandler::RIGHT, true);
		break;
	case ' ':
		inputHandler.setInputState(InputHandler::SPACE, true);
		break;
	case 'a':
	case 'A':
		inputHandler.setInputState(InputHandler::A, true);
		break;
	case 's':
	case 'S':
		inputHandler.setInputState(InputHandler::S, true);
		break;
	case 'd':
	case 'D':
		inputHandler.setInputState(InputHandler::D, true);
		break;
	case 'w':
	case 'W':
		inputHandler.setInputState(InputHandler::W, true);
		break;
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {

	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_UP:
		inputHandler.setInputState(InputHandler::UP, false);
		break;
	case OF_KEY_DOWN:
		inputHandler.setInputState(InputHandler::DOWN, false);
		break;
	case OF_KEY_LEFT:
		inputHandler.setInputState(InputHandler::LEFT, false);
		break;

	case OF_KEY_RIGHT:
		inputHandler.setInputState(InputHandler::RIGHT, false);
		break;
	case ' ':
		inputHandler.setInputState(InputHandler::SPACE, false);
		
		break;
	case 'a':
	case 'A':
		inputHandler.setInputState(InputHandler::A, false);
		
		break;
	case 's':
	case 'S':
		inputHandler.setInputState(InputHandler::S, false);
		break;
	case 'd':
	case 'D':
		inputHandler.setInputState(InputHandler::D, false);
		break;
	case 'w':
	case 'W':
		inputHandler.setInputState(InputHandler::W, false);
		break;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {



}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	// if moving camera, don't allow mouse interaction
//
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(glm::vec3(min.x, min.y, min.z), glm::vec3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(glm::vec3(origin.x, origin.y, origin.z), glm::vec3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(lander.getPosition(), cam.getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		} else {
			bLanderSelected = false;
		}
	}


}


//draw a box from a "Box" class  
//
void ofApp::drawBox(const Box &box) {
	glm::vec3 min = box.parameters[0];
	glm::vec3 max = box.parameters[1];
	glm::vec3 size = max - min;
	glm::vec3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x, center.y, center.z);
	float w = size.x;
	float h = size.y;
	float d = size.z;
	ofDrawBox(p, w, h, d);

}

// return a Mesh Bounding Box for the entire Mesh
//
Box ofApp::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	return Box(glm::vec3(min.x, min.y, min.z), glm::vec3(max.x, max.y, max.z));
}



//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// 
	//  implement your code here to drag the lander around
	//lander.setPosition(0, 0, 0);

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bLanderSelected = false;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}


//Collision Detection
void ofApp::collisionDetection() {
	if (terrain.overlap(lander.getHitbox())) {
		bCollision = true;
	} else {
		bCollision = false;
	}
}

void ofApp::land() {
	if (bCollision) {
		cout << "collision detected" << endl;
		bCollision = false;
	}

}

//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{ 5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
}

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

void ofApp::dragEvent(ofDragInfo dragInfo) {

}


//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	} else return glm::vec3(0, 0, 0);
}



bool ofApp::mouseIntersectPlane(glm::vec3 planePoint, glm::vec3 planeNorm, glm::vec3 &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}
