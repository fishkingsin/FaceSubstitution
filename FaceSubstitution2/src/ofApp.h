#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "Clone.h"
#include "ofxFaceTracker2.h"
#include "ofxDelaunay.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void dragEvent(ofDragInfo dragInfo);
	void loadFace(string face);
	
	void keyPressed(int key);

	ofxFaceTracker2 camTracker;
	ofVideoGrabber cam;
	
	ofxFaceTracker2 srcTracker;
	ofImage src;
    
	bool cloneReady;
	Clone clone;
	ofFbo srcFbo, maskFbo;

	ofDirectory faces;
	int currentFace;
    bool bDebug;
    

};
