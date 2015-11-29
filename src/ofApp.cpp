#include "ofApp.h"
#include "FontTools.h"
#include "Sampler.h"

/*
 compiler flags for PD:
 -DHAVE_UNISTD_H -DHAVE_ALLOCA_H -DUSEAPI_DUMMY -DPD -DLIBPD_EXTRA

 TODO:

 bug:

 */
//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetLogLevel(OF_LOG_SILENT);
    
    ofLogNotice() << "SphereTones::ofApp::setup";
    ofBackground(255, 255, 255);
    FontTools::loadFont("Roboto-Light.ttf", 64);    

    init();
}

void ofApp::exit()
{
    ofLogNotice() << "SphereTones::exit";
}

ofApp::~ofApp()
{
    sphereTones.save();
    ofLogNotice() << "SphereTones::ofApp destroyed";
}

void ofApp::init()
{
    ofEnableAlphaBlending();
    ofDisableDepthTest();
    ofSetVerticalSync(true);

#ifdef TARGET_ANDROID
    ofxRegisterMultitouch(this);
#endif

    sphereTones.mmToPixels = (getDensityDpi() / 25.4);
    
    Sampler::setup(this, "sfx");
    
    gDetector.setup(this);
    
    sphereTones.setup();
    
    ofAddListener(sphereTones.audioSavedEvents, this, &ofApp::audioSavedEvent);
}

int ofApp::getDensityDpi()
{
	int dpi = 96;
#ifdef TARGET_ANDROID
    JNIEnv *env = ofGetJNIEnv();
    jclass localClass = env->FindClass("cc/openframeworks/SphereTones/OFActivity");
    jclass javaClass = (jclass) env->NewGlobalRef(localClass);
    jmethodID method = env->GetStaticMethodID(javaClass, "getDensityDpi", "()I");
    dpi = env->CallStaticIntMethod(javaClass, method);
#endif
    return dpi;
}

float ofApp::getFreeSpace()
{
	float space = 10000;
#ifdef TARGET_ANDROID
    JNIEnv *env = ofGetJNIEnv();
    jclass localClass = env->FindClass("cc/openframeworks/SphereTones/OFActivity");
    jclass javaClass = (jclass) env->NewGlobalRef(localClass);
    jmethodID method = env->GetStaticMethodID(javaClass, "getFreeSpace", "()I");
    space = env->CallStaticIntMethod(javaClass, method);
#endif
    return space;
}

void ofApp::shareAudio(string path, string fileName)
{
#ifdef TARGET_ANDROID
	JNIEnv *env = ofGetJNIEnv();
	jclass localClass = env->FindClass("cc/openframeworks/SphereTones/OFActivity");
	jclass javaClass = (jclass) env->NewGlobalRef(localClass);

	jstring jstrPath = env->NewStringUTF(path.c_str());
	jstring jstrFileName = env->NewStringUTF(fileName.c_str());

	jmethodID method = env->GetStaticMethodID(javaClass, "shareAudio", "(Ljava/lang/String;Ljava/lang/String;)V");
	env->CallStaticVoidMethod(javaClass, method, jstrPath, jstrFileName);
#endif
}

void ofApp::audioSavedEvent(string & e)
{
	ofLogNotice() << "SphereTones::ofApp::audioSavedEvent: " << e;
	shareAudio(ofToDataPath("pd"), e);
}

//--------------------------------------------------------------
void ofApp::update()
{
    sphereTones.update();

    if (ofGetFrameNum() % 60 == 0)	// periodically check free disk space when recording
    {
		if (Sampler::isRecording() && getFreeSpace() < 1)   // if we have less than 1MB, stop recording
		{
			sphereTones.recording = false;
			Sampler::stopRecording();
		}
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    sphereTones.draw();
}

void ofApp::onTouch(ofVec2f p)
{
	ofLogNotice() << "SphereTones::ofApp::onTouch";
    sphereTones.onTouch(p);
}

void ofApp::onUndoDrag()
{
	ofLogNotice() << "SphereTones::ofApp::onUndoDrag";
    sphereTones.onUndoDrag();
}

void ofApp::onDrag(ofVec2f p)
{
	ofLogNotice() << "SphereTones::ofApp::onDrag";
    sphereTones.onDrag(p);
}

void ofApp::onRelease(ofVec2f p)
{
	ofLogNotice() << "SphereTones::ofApp::onRelease";
    sphereTones.onRelease(p);
}

void ofApp::onPan(ofVec2f p)
{
	ofLogNotice() << "SphereTones::ofApp::onPan";
    sphereTones.onPan(p);
}

void ofApp::onZoom(ofVec2f p, float amount)
{
	ofLogNotice() << "SphereTones::ofApp::onZoom";
    sphereTones.onZoom(p, amount);
}

//--------------------------------------------------------------

#ifndef TARGET_ANDROID

void ofApp::mouseScroll(int device, int axis, int delta)
{
    float scrollspeed = 0.001;
    sphereTones.onZoom(ofVec2f(ofGetMouseX(), ofGetMouseY()),  1 + delta * scrollspeed);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
    pMouseX = x;
    pMouseY = y;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
    if (button == 0)
        gDetector.onDrag(0, x, y);
    else if (button == 2)
        sphereTones.onPan(ofVec2f(x - pMouseX, y - pMouseY));
    
    pMouseX = x;
    pMouseY = y;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
    pMouseX = x;
    pMouseY = y;
    
    if (button == 0)
        gDetector.onTouch(0, x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
    pMouseX = x;
    pMouseY = y;
    
    if (button == 0)
        gDetector.onRelease(0, x, y);
}
#else

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id)
{
    gDetector.onTouch(id, x, y);
}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id)
{
    gDetector.onDrag(id, x, y);
}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id)
{
    gDetector.onRelease(id, x, y);
}
#endif
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
    //ofLog(OF_LOG_NOTICE, "width: %i height: %i numFrame: %i", ofGetWidth(), ofGetHeight(), ofGetFrameNum());
}

void ofApp::resume()
{
    ofLogNotice() << "SphereTones::ofApp::resume";
}

void ofApp::pause()
{
    ofLogNotice() << "SphereTones::ofApp::pause";
    gDetector.releaseAll();
    sphereTones.save();
}

void ofApp::stop()
{
    ofLogNotice() << "SphereTones::ofApp::stop";
    sphereTones.save();
}

bool ofApp::backPressed()
{
    ofLogNotice() << "SphereTones::ofApp::backPressed";
    return false;
}

void ofApp::unloadTextures()
{
    ofLogNotice() << "SphereTones::ofApp::unloadTextures";
    sphereTones.unloadTextures();
}

void ofApp::reloadTextures()
{
    ofLogNotice() << "SphereTones::ofApp::reloadTextures";
    sphereTones.loadTextures();
}

void ofApp::menuItemSelected(int menu_id)
{

}

bool ofApp::menuItemSelected(string menu_id_str)
{
    return false;
}

void ofApp::audioReceived(float * input, int bufferSize, int nChannels)
{
    Sampler::audioReceived(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::audioRequested(float * output, int bufferSize, int nChannels)
{
    Sampler::audioRequested(output, bufferSize, nChannels);
}

