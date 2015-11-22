#ifndef __SampleSelector_h__
#define __SampleSelector_h__

#include "ofMain.h"

class SampleSelectorEventArgs
{
public:
    int id;
    ofVec2f position;
};

class SampleSelector
{
public:
    SampleSelector();
    void setup(int _idMax);
    void update(ofVec2f screenTouchPoint, float _percent);
    void draw(bool inverz);
    void open(ofVec2f p);
    void onRelease();
    int getElementOverId(ofVec2f p);
    void arc(ofVec2f center, float angleMin, float angleMax, float radiusMin, float radiusMax, int res){float angleStep = (angleMax - angleMin) / (float)(res - 1);ofBeginShape();for (int i = 0; i < res; i++){float angle = i * angleStep;float x = center.x + cos(angle) * radiusMin;float y = center.y + sin(angle) * radiusMin;ofVertex(x, y);}for (int i = 0; i < res; i++){float angle = i * angleStep;float x = center.x + cos(angleMax - angle) * radiusMax;float y = center.y + sin(angleMax - angle) * radiusMax;ofVertex(x, y);}ofEndShape(true);};
    
    ofEvent<SampleSelectorEventArgs> onSampleSelected, onSamplePreview;
    bool alive;
    float radius, minRadius;
    ofVec2f center, touchPoint;
    int overId, pOverId, idMax;
    float percent;
    
};

#endif

