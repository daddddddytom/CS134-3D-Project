#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"
#include "box.h"
#include "ray.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "TransformObject.h"
#include <glm/gtx/intersect.hpp>
#include "EntityBase.h"
#include "ofxGui.h"
#include "Octree.h"

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
	bool raySelectWithOctree(ofVec3f &pointRet);

	void collisionDetection();

	ofEasyCam cam;
	ofCamera top;
	ofCamera *theCam;
	EntityBase lander;
	ofxAssimpModelLoader terrain;
	ofLight light;
	Box boundingBox;
	Box landerBounds;


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
	vector<Box> bboxList;

	ofVec3f selectedPoint;
	ofVec3f intersectPoint;

	glm::vec3 mouseDownPos;

	
	const float selectionRange = 4.0;


	
	
	Box testBox;
	
	
	Octree octree;
	TreeNode selectedNode;
	glm::vec3  mouseLastPos;
	bool bInDrag = false;
	ofxIntSlider numLevels;
	ofxPanel gui;
	//const float selectionRange = 4.0;





	TurbulenceForce* turbForce;
	GravityForce* gravityForce;
	ThrustForce* thrustForce;



};
