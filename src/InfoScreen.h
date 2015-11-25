#ifndef __InfoScreen_h__
#define __InfoScreen_h__

#include "ofMain.h"
#include "Timer.h"

class InfoScreen
{
public:
    InfoScreen();
    ~InfoScreen();
    
    void setup();
    bool onTouch(ofVec2f p);
    void update();
    void draw();
    void drawButton(ofRectangle view);
    void urlResponse(ofHttpResponse & e);
    void loadTextures();
    void unloadTextures();
    float width, height;
    
    float openPct;
    float damp;
    vector<ofTexture*> slides;
    vector<ofTexture*>::iterator currentSlide, pSlide;
    float currentAlpha;
    
    float slideStarted;
    float slideInterval;
    
    ofRectangle button;
    bool closing;
};

#endif

