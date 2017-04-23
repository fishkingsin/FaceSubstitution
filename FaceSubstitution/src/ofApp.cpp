#include "ofApp.h"

using namespace ofxCv;

void ofApp::setup() {
#ifdef TARGET_OSX
	//ofSetDataPathRoot("../data/");
#endif
	ofSetVerticalSync(true);
	cloneReady = false;
	cam.initGrabber(1280, 720);
	clone.setup(cam.getWidth(), cam.getHeight());
	ofFbo::Settings settings;
	settings.width = cam.getWidth();
	settings.height = cam.getHeight();
	maskFbo.allocate(settings);
	srcFbo.allocate(settings);
	camTracker.setup();
	srcTracker.setup();
	srcTracker.setIterations(25);
	srcTracker.setAttempts(4);

	faces.allowExt("jpg");
	faces.allowExt("png");
	faces.listDir("faces");
	currentFace = 0;
	if(faces.size()!=0){
		loadFace(faces.getPath(currentFace));
	}
}

void ofApp::update() {
	cam.update();
	if(cam.isFrameNew()) {
		camTracker.update(toCv(cam));
		
		cloneReady = camTracker.getFound();
		if(cloneReady) {
			ofMesh camMesh = camTracker.getImageMesh();
			camMesh.clearTexCoords();
			camMesh.addTexCoords(srcPoints);
			
			maskFbo.begin();
			ofClear(0, 255);
			camMesh.draw();
			maskFbo.end();
			
			srcFbo.begin();
			ofClear(0, 255);
			src.bind();
			camMesh.draw();
			src.unbind();
			srcFbo.end();
			
			clone.setStrength(16);
			clone.update(srcFbo.getTexture(), cam.getTexture(), maskFbo.getTexture());
		}
	}
}

void ofApp::draw() {
	ofSetColor(255);
	
	if(src.getWidth() > 0 && cloneReady) {
		clone.draw(0, 0);
        if(bDebug){
            camTracker.getImageMesh().drawWireframe();
            vector<ofVec2f>points = camTracker.getImagePoints();
            int i = 0 ;
            for(auto point : points){
                ofDrawBitmapString(ofToString(i), point.x, point.y);
                i++;
            }
        }
	} else {
		cam.draw(0, 0);
	}
	
	if(!camTracker.getFound()) {
		ofDrawBitmapString("camera face not found", 10, 10);
	}
	if(src.getWidth() == 0) {
		ofDrawBitmapString("drag an image here", 10, 30);
	} else if(!srcTracker.getFound()) {
		ofDrawBitmapString("image face not found", 10, 30);
	}
}

void ofApp::loadFace(string face){
	src.load(face);
	if(src.getWidth() > 0) {
		srcTracker.update(toCv(src));
		srcPoints = srcTracker.getImagePoints();
	}
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
	loadFace(dragInfo.files[0]);
}

void ofApp::keyPressed(int key){
	switch(key){
        case OF_KEY_TAB:
            bDebug = !bDebug;
            break;
	case OF_KEY_UP:
		currentFace++;
		break;
	case OF_KEY_DOWN:
		currentFace--;
		break;
	}
	currentFace = ofClamp(currentFace,0,faces.size()-1);
	if(faces.size()!=0){
		loadFace(faces.getPath(currentFace));
	}
}
