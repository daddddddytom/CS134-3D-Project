#include "ofApp.h"
#include "Util.h"
#include "Octree.h"
#include <glm/gtx/intersect.hpp>

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup() {
	ofBackground(ofColor::black);

	pathPoints = { glm::vec3(0, 0, 0), glm::vec3(0, 10, 0), glm::vec3(10, 50, 10), glm::vec3(20, 100, 20), glm::vec3(40, 200, 40), glm::vec3(80, 250, 80),
		glm::vec3(160, 270, 160), glm::vec3(180, 290, 180), glm::vec3(200, 300, 200) };

	gravityForce = new GravityForce(ofVec3f(0, 10, 0));

	pathBox = Box(glm::vec3(0, 0, 0), glm::vec3(200, 300, 200));

	background.load("geo/starfield-8.png");

#ifdef TARGET_OPENGLES
	shader.load("shaders_gles/shader");
#else
	shader.load("shaders/shader");
#endif
	if (!ofLoadImage(particleTex, "images/dot.png")) {
		cout << "Particle Texture File: images/dot.png not found" << endl;
		ofExit();
	}

	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
	//	ofSetWindowShape(1024, 768);

	//camera setup
	cam.setPosition(lander.getPosition().x, lander.getPosition().y - 50, lander.getPosition().z);
	cam.rotate(-45, glm::vec3(1, 0, 0));
	cam.setDistance(500);
	cam.setNearClip(.1);
	cam.setFov(100);   // approx equivalent to 28mm in 35mm format
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

	plane.set(100000, 100000);   ///dimensions for width and height in pixels
	plane.setPosition(0, Box::meshBounds(terrain.getMesh(0)).min().y, 0); /// position in x y z
	plane.setResolution(2, 2);
	plane.rotateDeg(90, 1, 0, 0);
	plane.enableColors();
	plane.enableTextures();
	ofDisableArbTex();
	ofLoadImage(texture, "geo/martianfloor.png");

	gameState = MAIN_MENU;

	explosion = new ParticleEmitter();
	turbulanceForce = new TurbulenceForce(ofVec3f(-20, -20, 0), ofVec3f(20, 20, 0));
	gravForce = new GravityForce(ofVec3f(0, -20, 0));
	radialForce = new ImpulseRadialForce(1000.0);
	explosion->sys->addForce(turbulanceForce);
	explosion->sys->addForce(gravForce);
	explosion->sys->addForce(radialForce);
	explosion->setVelocity(ofVec3f(200, 200, 0));
	explosion->setOneShot(true);
	explosion->setEmitterType(RadialEmitter);
	explosion->setGroupSize(30);
	explosion->setParticleRadius(1);
	explosion->setLifespan(1);

}

void ofApp::initGame() {
	cam.setPosition(lander.getPosition().x, lander.getPosition().y - 50, lander.getPosition().z);
	cam.rotate(-45, glm::vec3(1, 0, 0));
	cam.setDistance(500);
	cam.setNearClip(.1);
	cam.setFov(100);   // approx equivalent to 28mm in 35mm format
	cam.disableMouseInput();
	top.setNearClip(.1);
	top.setFov(80);
	trackCam.setPosition(200, 300, 500);
	trackCam.setNearClip(.1);
	trackCam.setFov(40);

	bExplode = false;
	lander.setPosition(200, 300, 200);
	lander.set_velocity(glm::vec3(-120, 20, -60));
	lander.set_fuel(120);
	lander.rotationX = 0;
	lander.rotationY = 0;
	lander.rotationZ = 0;
	lander.updateHitbox();
	score = 0;
	gameState = IN_GAME;
}

//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
	switch (gameState) {
	case MAIN_MENU:
		break;
	case IN_GAME:
		landerLight.setPosition(lander.getPosition().x, lander.getPosition().y + 50, lander.getPosition().z);

		// check input
		if (inputHandler.getInputState(InputHandler::SPACE) && lander.get_fuel() > 0.0) {
			lander.thrusterOn = true;
			float time = ofGetElapsedTimeMillis();
			//cout << time - lander.mainThruster.lastSpawned << endl;
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



		//updating cameras
		top.setPosition(lander.getPosition().x, lander.getPosition().y - 1, lander.getPosition().z);
		top.lookAt(glm::vec3(lander.getPosition().x, 0, lander.getPosition().z));
		trackCam.lookAt(lander.getPosition());
		trackCam.setPosition(lander.getPosition().x - 100, lander.getPosition().y + 100, lander.getPosition().z);

		// collision check
		if ((terrain.overlap(lander.getHitbox()) || lander.getAltitude(terrain) < 0)) {
			glm::vec3 normal = glm::vec3(0, 1, 0);
			glm::vec3 impulseForce = (2) * (glm::dot(-lander.get_velocity(), normal) * normal);
			lander.addForce(impulseForce);
			if (lander.isUpright(lander.head()) && glm::length(lander.get_velocity()) < 3.0f) {
				score = int(lander.get_fuel() / 100 + 1000 - (glm::length(lander.get_velocity()) * 500) + 100 - glm::length(pathPoints[pathPoints.size() - 1] - lander.getPosition()));
			} else if (!bExplode) {
				explosion->setPosition(lander.getPosition());
				explosion->sys->reset();
				explosion->start();
				lander.explode();
				bExplode = true;
				score = -1000;
			}
			gameState = END_SCREEN;

		} else {
			lander.addForce(glm::vec3(0, -7, 0));
		}
	case END_SCREEN:
		landerLight.setPosition(lander.getPosition().x, lander.getPosition().y + 50, lander.getPosition().z);
		explosion->update();
		if (gameState != IN_GAME) {
			lander.isActive = false;
		} else {
			lander.isActive = true;
		}
		lander.update();

		break;
	}


}


//--------------------------------------------------------------
void ofApp::draw() {
	switch (gameState) {
	case MAIN_MENU:
		// draw welcome message
		ofDrawBitmapString("Press ENTER to start the simulation.", ofGetWindowWidth() / 2 - 125, ofGetWindowHeight() / 2);
		break;
	case END_SCREEN:
	case IN_GAME:
		loadVbo();
		ofPushMatrix();
		ofDisableDepthTest();
		ofSetColor(255, 255, 255);
		ofScale(2, 2);
		background.draw(-200, -100);
		ofEnableDepthTest();
		ofPopMatrix();
		ofSetColor(255, 255, 255);

		theCam->begin();
		//explosion.draw();
		ofPushMatrix();

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
			terrain.octree.drawLeafNodes();

		}
		lander.draw();
		/*
		terrain.octree.drawBox(lander.getHitbox());
		ofSetColor(ofColor::blue);
		ofDrawSphere(lander.getHitbox().min(), 0.5);
		ofSetColor(ofColor::red);
		ofDrawSphere(lander.getHitbox().max(), 0.5);
		*/
		// recursively draw octree
		//
		ofDisableLighting();
		explosion->draw();
		//	ofNoFill();
		if (explosion->started) {

			glDepthMask(GL_FALSE);

			ofSetColor(ofColor::aquamarine);

			// this makes everything look glowy :)
			//
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			ofEnablePointSprites();

			// begin drawing in the camera
			shader.begin();
			//theCam->begin();

			// draw particle emitter here..

			particleTex.bind();

			vbo.draw(GL_POINTS, 0, (int)explosion->sys->particles.size());
			//explosion->draw();
			particleTex.unbind();

			//  end drawing in the camera
			//
			//theCam->end();
			shader.end();

			ofDisablePointSprites();
			ofDisableBlendMode();
			ofEnableAlphaBlending();

			// set back the depth mask
			//
			glDepthMask(GL_TRUE);
		}


		float minDistance = glm::length(pathPoints[pathPoints.size() - 1] - lander.getPosition());
		for (int i = 0; i < pathPoints.size() - 1; i++) {
			float dis = glm::length(pathPoints[i] - lander.getPosition());
			ofDrawLine(pathPoints[i], pathPoints[i + 1]);
			if (dis < minDistance) {
				minDistance = dis;
			}
		}

		ofPopMatrix();
		//cam.end();
		theCam->end();
		glDepthMask(false);
		//gui.draw();
		if (gameState == IN_GAME) {
			ofDrawBitmapString("Altitude (agl): " + std::to_string(lander.getAltitude(terrain)), ofGetWindowWidth() - 200, 15);
			ofDrawBitmapString("Fuel remaining: " + std::to_string(lander.get_fuel()), ofGetWindowWidth() - 200, 30);
			ofDrawBitmapString("Speed: " + std::to_string(glm::length(lander.get_velocity())), ofGetWindowWidth() - 200, 45);
			ofDrawBitmapString("Distance from path: " + std::to_string(minDistance), ofGetWindowWidth() - 200, 60);
		} else {
			// draw end screen stuff
			ofDrawBitmapString("Simulation over.", ofGetWindowWidth() / 2 - 75, ofGetWindowHeight() / 2);
			// draw score
			ofDrawBitmapString("Score: " + to_string(score), ofGetWindowWidth() / 2 - 50, ofGetWindowHeight() / 2 + 25);
			if (score < 0) {
				ofDrawBitmapString("Landing Failed.", ofGetWindowWidth() / 2 - 75, ofGetWindowHeight() / 2 + 50);
			} else {
				ofDrawBitmapString("Landing Successful.", ofGetWindowWidth() / 2 - 75, ofGetWindowHeight() / 2 + 50);
			}
		}
		glDepthMask(true);
		break;
	}


}

void ofApp::loadVbo() {
	if (explosion->sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < explosion->sys->particles.size(); i++) {
		points.push_back(explosion->sys->particles[i].position);
		sizes.push_back(ofVec3f(explosion->particleRadius));
	}



	int total = (int)points.size();
	vbo.clear();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
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
			initGame();
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

	// if rover is loaded, test for selection
	//
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

	//implement your code here to drag the lander around
	//lander.setPosition(0, 0, 0);
	glm::vec3 origin = theCam->getPosition();
	glm::vec3 camAxis = theCam->getZAxis();
	glm::vec3 mouseWorld = theCam->screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;
	if (bInDrag&&bLanderSelected) {
		lander.set_velocity(glm::vec3(0, 0, 0));
		glm::vec3 landerPos = lander.getPosition();
		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z); mouseLastPos = mousePos;
	}

	bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
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

