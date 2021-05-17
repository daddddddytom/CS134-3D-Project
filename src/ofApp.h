#pragma once

#include "ofMain.h"
#include "Box.h"
#include "Ray.h"
#include "ParticleSystem.h"
#include <glm/gtx/intersect.hpp>
#include "EntityLander.h"
#include "InputHandler.h"
#include "ofxGui.h"
#include "Terrain.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void drawAxis(ofVec3f);
	void initLightingAndMaterials();
	void savePicture();
	void toggleWireframeMode();
	void togglePointsDisplay();
	void toggleSelectTerrain();
	void setCameraTarget();
	bool mouseIntersectPlane(glm::vec3 planePoint, glm::vec3 planeNorm, glm::vec3 &point);
	glm::vec3 getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm);
	void drawBox(const Box &box);
	Box meshBounds(const ofMesh &);

	void collisionDetection();

	ofEasyCam cam;
	ofCamera top;
	ofCamera *theCam;
	ofCamera trackCam;

	EntityLander lander = EntityLander("geo/landerTest1.obj");
	Terrain terrain = Terrain("geo/terrain.obj", 7);

	ofLight light;


	vector<Box> colBoxList;
	bool pointSelected = false;
	bool bAltKeyDown;
	bool bCtrlKeyDown;
	bool bWireframe;
	bool bDisplayPoints;
	bool bPointSelected;
	bool bCollision = false;
	bool bLanderLoaded = false;
	bool bTerrainSelected;
	bool bLanderSelected = false;
	bool bDisplayBBoxes = false;
	bool bDisplayOctree = false;
	vector<Box> bboxList;
	void checkDistToPath();
	glm::vec3 selectedPoint;
	glm::vec3 intersectPoint;

	glm::vec3 mouseDownPos;

	void land();
	const float selectionRange = 4.0;


	bool bLanderOut;
	Box testBox;
	Box pathBox;
	
	glm::vec3  mouseLastPos;
	bool bInDrag = false;
	bool startEngine = false;
	ofxIntSlider numLevels;
	ofxPanel gui;
	//const float selectionRange = 4.0;

	void drawPath(Box landerBox);

	TurbulenceForce* turbForce;
	GravityForce* gravityForce;
	ThrustForce* engineForce;

	InputHandler inputHandler;

};
