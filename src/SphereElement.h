#ifndef __SphereElement_h__
#define __SphereElement_h__

#include "ofMain.h"
#include "Timer.h"

class SphereElement
{
public:
    SphereElement(ofVec2f _sphereCenter, ofVec2f p, float _sliderPercent, int _started, int _sampleId);
    ~SphereElement();
    
    void update();
    void draw();
    void drawRadialText();
    
    void setVolume(float _volume);
    void updatePositions(ofVec2f p);
    void bang();
    
    void onUndoDrag();
    void onRelease();
    
    void setBall(ofVec2f p);
    void setSlider(ofVec2f p);
    
    void clampLength();
    void clampSliderPercent();
    
    float getFrequency(int note);
    float getQuanted(float value, float step);
    
    
    ofVec2f sliderStart, sliderEnd, ballStart, ballEnd;
    ofVec2f ball, slider, sphereCenter;
    
    bool alive;
    int sampleId;
    int note;
    float volume;
    
    bool draggingSlider, draggingBall;
    
    float pAngle, pLength;
    int pStarted;
    float pSliderPercent;
    
    float angle, length;
    float sliderPercent;
    float sliderLength;
    int started;
    float pathPercent;
    bool pathPositive;
    bool banged;
    bool enable;
    
    int bangAnimInterval;
    Timer bangAnimTimer;
    
    vector<string> notes;    
    
};

#endif

