#include "ofApp.h"
#include "FontTools.h"
#include "Sampler.h"

/*
 change : ofFmodSoundPlayer.cpp
 FMOD_System_Init(sys, 512, FMOD_INIT_NORMAL, NULL);  //do we want just 32 channels?
 
 pd flags:
 -DHAVE_UNISTD_H -DHAVE_ALLOCA_H -DUSEAPI_DUMMY -DPD -DLIBPD_EXTRA

 TODO:

 */
//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetLogLevel(OF_LOG_SILENT);
    
    ofLogNotice() << "SphereTones::ofApp::setup";
    ofBackground(255, 255, 255);
    FontTools::loadFont("verdana.ttf", 64);    

    init();
    
    //ofHideCursor();
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
	gDetector.releaseAll();
}

void ofApp::audioSavedEvent(string & e)
{
	ofLogNotice() << e;
	shareAudio(ofToDataPath("pd"), e);
}

//--------------------------------------------------------------
void ofApp::update()
{
    sphereTones.update();

    if (ofGetFrameNum() % 60 == 0)	// check free disk space when recording
    {
		if (Sampler::isRecording() && getFreeSpace() < 1)
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
    sphereTones.onTouch(p);
}

void ofApp::onUndoDrag()
{
    sphereTones.onUndoDrag();
}

void ofApp::onDrag(ofVec2f p)
{
    sphereTones.onDrag(p);
}

void ofApp::onRelease(ofVec2f p)
{
    sphereTones.onRelease(p);
}

void ofApp::onPan(ofVec2f p)
{
    sphereTones.onPan(p);
}

void ofApp::onZoom(ofVec2f p, float amount)
{
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
    
    /*
    if (key == 's')
    {
        ofSaveScreen(ofGetTimestampString() + ".png");
    }
    if (key == 'f')
    {
        ofToggleFullscreen();
        ofHideCursor();
    }*/
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
    //ofLogNotice() << "SphereTones::ofApp::menuItemSelected";
    //Sampler::setNumOfChannels(menu_id);
}

bool ofApp::menuItemSelected(string menu_id_str)
{
    //ofLog(OF_LOG_NOTICE,"menuSelected: %s", menu_id_str.c_str());
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

