#pragma once

#include "ofMain.h"
#include "GestureDetector.h"
#include "SphereTones.h"

#ifdef TARGET_ANDROID
#include "ofxAndroid.h"
class ofApp : public ofxAndroidApp, GestureListener
#else
#include "ofxManyMouse.h"
class ofApp : public ofBaseApp, GestureListener, ofxManyMouse
#endif
{
public:
    ~ofApp();
    void setup();
    void init();
    void update();
    void draw();
    void exit();
    void pause();
    void resume();
    void stop();
    
    void reloadTextures();
    void unloadTextures();

    bool backPressed();
    bool menuItemSelected(string menu_id_str);
    void menuItemSelected(int menu_id);
    
    void windowResized(int w, int h);
    
    void onTouch(ofVec2f p);
    void onUndoDrag();
    void onDrag(ofVec2f p);
    void onRelease(ofVec2f p);
    void onPan(ofVec2f p);
    void onZoom(ofVec2f p, float amount);
    
    void touchDown(int x, int y, int id);
    void touchMoved(int x, int y, int id);
    void touchUp(int x, int y, int id);
    
    void audioReceived(float * input, int bufferSize, int nChannels);
    void audioRequested(float * output, int bufferSize, int nChannels);

    void audioSavedEvent(string & e);
        
    int getDensityDpi();
    void shareAudio(string path, string fileName);
    float getFreeSpace();

    GestureDetector gDetector;
    SphereTones sphereTones;

#ifndef TARGET_ANDROID
    ofxManyMouse manyMouse;
    void mouseScroll(int device, int axis, int delta);
    
    int pMouseX, pMouseY;
    void keyPressed(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
#endif    
    
    ofTexture texHand;
};
