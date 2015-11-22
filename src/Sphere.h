#ifndef __Sphere_h__
#define __Sphere_h__

#include "ofMain.h"
#include "SphereElement.h"
#include "Timer.h"

class Sphere
{
public:
    Sphere(ofVec2f p, int _sampleId);
    ~Sphere();
    
    void update();
    void draw();
    void drawInside(float scale);
    void drawOutside(float scale);
    void drawLocation();
    void drawRadialText();
    void drawLocationBox(ofRectangle view);
    
    void onTouch(ofVec2f p);
    void onUndoDrag();
    void onDrag(ofVec2f p);
    void onRelease(ofVec2f p);
    
    void move(ofVec2f p);
    void calcBBox();
    void setVolume(float _volume);
    void setSampleId(int id);
    
    void save(ofXml * xml);
    void load(ofXml * xml);
    
    SphereElement * addElement(ofVec2f p, float sliderPercent, int started);
    SphereElement * getElementByBall(ofVec2f p, float radiusMultiply);
    SphereElement * getElementBySlider(ofVec2f p, float radiusMultiply);
    
    bool isPointOver(ofVec2f p, float radiusMultiply);
    bool isPointOverCenter(ofVec2f p, float radiusMultiply);
    bool isPointOverElement(ofVec2f p, float radiusMultiply);
    
    ofVec2f center;
    int sampleId;
    
    bool alive;
    
    vector<SphereElement*> elements;
    SphereElement * draggedElement;
    SphereElement * newElement;
    
    ofVec2f dragShift;
    ofVec2f touchPosition;
    bool dragging;
    ofVec2f bboxP1, bboxP2;
    bool visible;
    Timer creationTimer;
};

#endif

