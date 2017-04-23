#include "ofApp.h"

using namespace ofxCv;

void ofApp::setup() {
//#ifdef TARGET_OSX
//	//ofSetDataPathRoot("../data/");
//#endif
	ofSetVerticalSync(true);
	cloneReady = false;
	cam.initGrabber(1280, 720);
	clone.setup(cam.getWidth(), cam.getHeight());
	ofFbo::Settings settings;
	settings.width = cam.getWidth();
	settings.height = cam.getHeight();
	maskFbo.allocate(settings);
	srcFbo.allocate(settings);
    string modelPath = ofToDataPath("model/shape_predictor_68_face_landmarks.dat",true);
	camTracker.setup(modelPath);
	srcTracker.setup(modelPath);
//	srcTracker.setIterations(25);
//	srcTracker.setAttempts(4);

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
		
        
        
        cloneReady = camTracker.getInstances().size() > 0;
		if(cloneReady) {
            
            if(src.getWidth() > 0) {
            
                srcTracker.update(toCv(src));
               
            }
            
            vector<ofxFaceTracker2Instance> instances = camTracker.getInstances();
            if(instances.size() > 0){
                
                
                ofxFaceTracker2Instance instance = instances[0];
                
                ofMesh camMesh = instance.getLandmarks().getImageMesh();
                
                

                if(srcTracker.getInstances().size() > 0 ){
                
                    
                    ofMesh mesh = srcTracker.getInstances()[0].getLandmarks().getImageMesh();

                    vector<ofVec2f>points;
                    for (auto p : mesh.getVertices()){
                        points.push_back(p);
                    }

                    camMesh.clearTexCoords();
                    camMesh.addTexCoords(points);
                    
                    
                }


                
                
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
                clone.update(srcFbo.getTexture(), cam.getTexture(),maskFbo.getTexture());
            }
        }
	}
}

void ofApp::draw() {
	ofSetColor(255);
    
	if(src.getWidth() > 0 && cloneReady) {
		clone.draw(0, 0);
        if(bDebug){
            
            camTracker.drawDebug(0, 0, ofGetWidth(), ofGetHeight());
            vector<ofxFaceTracker2Instance> instances = camTracker.getInstances();
            if(instances.size() > 0){
                ofxFaceTracker2Instance instance = instances[0];
                ofMesh camMesh = instance.getLandmarks().getImageMesh();
                camMesh.drawWireframe();
                int index = 0;
                for(auto v : instance.getLandmarks().getImagePoints()){
                    ofDrawBitmapString(ofToString(index), v.x, v.y);
                    index++;
                }
            }
            srcTracker.drawDebug(0, 0, src.getWidth(), src.getHeight());
            vector<ofxFaceTracker2Instance> instances2  = srcTracker.getInstances();
            if(instances2.size() > 0){
                ofxFaceTracker2Instance instance = instances2[0];
                ofMesh camMesh = instance.getLandmarks().getImageMesh();
                
                
                
                vector<ofVec2f> points;
                
                for(auto v : camMesh.getVertices()){
                    points.push_back(v);
                }

                camMesh.clearTexCoords();
                camMesh.addTexCoords(points);
                
                ofSetColor(ofColor::white);
                src.bind();
                camMesh.draw();
                src.unbind();
                
                int index = 0;
                for(auto v : instance.getLandmarks().getImagePoints()){
                    ofDrawBitmapString(ofToString(index), v.x, v.y);
                    index++;
                }
                
                
            }
        }
	} else {
		cam.draw(0, 0);
	}
	
//	if(!camTracker.getFound()) {
//		ofDrawBitmapString("camera face not found", 10, 10);
//	}
	if(src.getWidth() == 0) {
		ofDrawBitmapString("drag an image here", 10, 30);
	}
//    else if(!srcTracker.getFound()) {
//		ofDrawBitmapString("image face not found", 10, 30);
//	}
}

void ofApp::loadFace(string face){
	src.load(face);
    
	if(src.getWidth() > 0) {
//		srcTracker.update(toCv(src));
//        if(srcTracker.getInstances().size() > 0 ){
//            srcPoints = srcTracker.getInstances()[0].getLandmarks().getImagePoints();
//        }
////		srcPoints = srcTracker.getImagePoints();
//        srcImageFbo.clear();
//        srcImageFbo.allocate(src.getWidth(), src.getHeight());
//        
//        srcImageFbo.begin();
//        src.draw(0, 0,src.getWidth(), src.getHeight());
//        srcImageFbo.end();
        srcTracker.update(toCv(src));
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
    if(OF_KEY_UP == key || OF_KEY_DOWN == key){
        currentFace = ofClamp(currentFace,0,faces.size()-1);
        if(faces.size()!=0){
            loadFace(faces.getPath(currentFace));
        }
    }
}
