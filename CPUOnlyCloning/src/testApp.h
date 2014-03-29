#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	ofImage src, dst, mask;
	ofImage srcBlur, dstBlur;
    ofImage result;
    ofShader cloneShader;
};