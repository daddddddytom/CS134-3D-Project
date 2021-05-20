#include "ofApp.h"
#include "Util.h"
#include "Octree.h"
#include <glm/gtx/intersect.hpp>

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup() {
	ofBackground(ofColor::black);
	
	gravityForce = new GravityForce(ofVec3f(0, 10, 0));

	lander.setPosition(200, 300, 200);

	pathBox = Box(glm::vec3(0, 0, 0), glm::vec3(200, 300, 200));

	background.load("geo/starfield-8.png");
	
#ifdef TARGET_OPENGLES
	shader.load("shaders_gles/shader");
#else
	shader.load("shaders/shader");
#endif

	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
	//	ofSetWindowShape(1024, 768);

	//camera setup
	cam.setPosition(0, 50, 0);
	cam.rotate(-45, glm::vec3(1,0,0));
	cam.setDistance(500);
	cam.setNearClip(.1);
	cam.setFov(100);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	top.setNearClip(.1);
	top.setFov(80);
	trackCam.setPosition(200, 300, 500);
	trackCam.setNearClip(.1);
	trackCam.setFov(40);

	theCam = &cam;

	ofLight light;
	ofLight keyLight, landerLight;

	ofEnableSmoothing();
	ofEnableDepthTest();
	//cam.setPosition(200, 400, 200);

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();


	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));

	plane.set(100000, 100000);   ///dimensions for width and height in pixels
	plane.setPosition(0, -15, 0); /// position in x y z
	plane.setResolution(2, 2);
	plane.rotateDeg(83.5, 1, 0, 0);
	plane.enableColors();
	plane.enableTextures();
	ofDisableArbTex();
	ofLoadImage(texture, "geo/testmartialfloor.png");

	gameState = MAIN_MENU;
}

//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
	switch(gameState) {
	case MAIN_MENU: 
		
		break;
	case IN_GAME: 
		checkDistToPath();
		if (bLanderOut) {
			cout << "Mission failed: Lander too far way from path" << endl;
		}

		landerLight.setPosition(lander.getPosition().x, lander.getPosition().y + 50, lander.getPosition().z);
		// check input
		if (inputHandler.getInputState(InputHandler::SPACE) && lander.get_fuel() > 0.0) {
			lander.thrusterOn = true;
			float time = ofGetElapsedTimeMillis();
			cout << time - lander.mainThruster.lastSpawned << endl;
			if ((time - lander.mainThruster.lastSpawned) >= 100)
				lander.set_fuel(lander.get_fuel() - 0.1);
		} else {
			lander.thrusterOn = false;
		}

		lander.rotateZACW = inputHandler.getInputState(InputHandler::A);
		lander.rotateZCW = inputHandler.getInputState(InputHandler::D);
		lander.rotateXACW = inputHandler.getInputState(InputHandler::W);
		lander.rotateXCW = inputHandler.getInputState(InputHandler::S);
		lander.rotateYACW = inputHandler.getInputState(InputHandler::Q);
		lander.rotateYCW = inputHandler.getInputState(InputHandler::E);
		lander.XLthrusterOn = inputHandler.getInputState(InputHandler::LEFT);
		lander.XRthrusterOn = inputHandler.getInputState(InputHandler::RIGHT);
		lander.ZLthrusterOn = inputHandler.getInputState(InputHandler::UP);
		lander.ZRthrusterOn = inputHandler.getInputState(InputHandler::DOWN);

		//lander.addForce(glm::vec3(0, -7, 0));

		//updating cameras
		top.setPosition(lander.getPosition().x, lander.getPosition().y - 1, lander.getPosition().z);
		top.lookAt(glm::vec3(lander.getPosition().x, 0, lander.getPosition().z));
		trackCam.lookAt(lander.getPosition());
		collisionDetection();
		lander.update();
		land();
		break;
	case END_SCREEN: 
		
		break;
	}
	
	
}


//--------------------------------------------------------------
void ofApp::draw() {
	switch (gameState) {
	case MAIN_MENU:
		// draw welcome message
		ofDrawBitmapString("Press ENTER to start the simulation.", ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
		break;
	case IN_GAME: 
		ofPushMatrix();
		ofDisableDepthTest();
		ofSetColor(255, 255, 255);
		ofScale(2, 2);
		background.draw(-200, -100);
		ofEnableDepthTest();
		ofPopMatrix();

		glDepthMask(false);
		gui.draw();
		ofDrawBitmapString("Altitude (agl): " + std::to_string(lander.getAltitude(terrain)), ofGetWindowWidth() - 200, 15);
		ofDrawBitmapString("Fuel remaining: " + std::to_string(lander.get_fuel()), ofGetWindowWidth() - 200, 30);
		glDepthMask(true);
		ofSetColor(255, 255, 255);

		theCam->begin();

		ofPushMatrix();
		drawPath(lander.getHitbox());

		ofEnableLighting();              // shaded mode
		terrain.drawFaces();
		ofSetColor(110, 36, 3);
		texture.bind();
		plane.draw();
		texture.unbind();
		ofSetColor(255, 255, 255);

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

		lander.draw();
		// recursively draw octree
		//
		ofDisableLighting();
		//	ofNoFill();


		ofPopMatrix();
		//cam.end();
		theCam->end();
		shader.begin();
		theCam->begin();

		theCam->end();
		shader.end();
		break;
	case END_SCREEN:
		// draw end screen stuff
		// draw end game message
		//TODO: DRAW BASED ON RESULT
		ofDrawBitmapString("Simulation over.", ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
		// draw score
		ofDrawBitmapString("Score:", ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 100);
		break;
	}
			
	
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
	//cout << ofGetKeyPressed() << endl;
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
	case 'R':

		cam.reset();
		cam.rotate(-45, glm::vec3(1, 0, 0));
		cam.setPosition(0, 50, 0);
		cam.setDistance(500);
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
	case OF_KEY_F3:
		theCam = &trackCam;
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
	case OF_KEY_RETURN:
		if (gameState == MAIN_MENU) {
			gameState = IN_GAME;
		} else if (gameState == END_SCREEN) {
			gameState = MAIN_MENU;
		}
		break;
	case ' ':
		if (lander.get_fuel() > 0.0) {
			inputHandler.setInputState(InputHandler::SPACE, true);
			lander.thrusterStartTime = ofGetElapsedTimeMillis();
		}
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
	case 'q':
	case 'Q':
		inputHandler.setInputState(InputHandler::Q, true);
		break;
	case 'e':
	case 'E':
		inputHandler.setInputState(InputHandler::E, true);
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
	case 'q':
	case 'Q':
		inputHandler.setInputState(InputHandler::Q, false);
		break;
	case 'e':
	case 'E':
		inputHandler.setInputState(InputHandler::E, false);
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

	ofVec3f pos = lander.getPosition();
	keyLight.setup();
	keyLight.enable();
	keyLight.setAreaLight(2, 2);
	keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	keyLight.setDiffuseColor(ofFloatColor(0.9, 0.9, 0.9));
	keyLight.setSpecularColor(ofFloatColor(1, 0, 0));
	keyLight.rotate(180, ofVec3f(1, 0, 0));
	keyLight.setPosition(0, 100, 0);

	landerLight.setup();
	landerLight.enable();
	landerLight.setSpotlight();
	landerLight.setScale(.1);
	landerLight.setSpotlightCutOff(200);
	landerLight.setAttenuation(2, .00001, .00001);
	landerLight.setAmbientColor(ofFloatColor(0, 0, 0));
	landerLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	landerLight.setSpecularColor(ofFloatColor(1, 1, 1));
	landerLight.rotate(-90, ofVec3f(1, 0, 0));
	landerLight.setPosition(pos.x, pos.y, pos.z);

	landerLight2.setup();
	landerLight2.enable();
	landerLight2.setSpotlight();
	landerLight2.setScale(.1);
	landerLight2.setSpotlightCutOff(200);
	landerLight2.setAttenuation(2, .00001, .00001);
	landerLight2.setAmbientColor(ofFloatColor(0.1, 0, 0));
	landerLight2.setDiffuseColor(ofFloatColor(0.9, 0.9, 0.9));
	landerLight2.setSpecularColor(ofFloatColor(0.9, 0.9, 0.9));
	landerLight2.rotate(45, ofVec3f(0, 0, 1));
	landerLight2.setPosition(0, 100, 100);

	landerLight3.setup();
	landerLight3.enable();
	landerLight3.setSpotlight();
	landerLight3.setScale(.1);
	landerLight3.setSpotlightCutOff(200);
	landerLight3.setAttenuation(2, .00001, .00001);
	landerLight3.setAmbientColor(ofFloatColor(0.1, 0, 0));
	landerLight3.setDiffuseColor(ofFloatColor(0.9, 0.9, 0.9));
	landerLight3.setSpecularColor(ofFloatColor(0.9, 0.9, 0.9));
	landerLight3.rotate(-45, ofVec3f(0, 0, 1));
	landerLight3.setPosition(0, 100, 0);


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


