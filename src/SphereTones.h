#ifndef __SphereTones_h__
#define __SphereTones_h__

#include "ofMain.h"

#include "TangibleView.h"
#include "Timer.h"
#include "Sphere.h"
#include "SphereElement.h"
#include "SampleSelector.h"
#include "InfoScreen.h"

class SphereTones
{
public:
    enum
    {
        ZOOM_CLOSE,
        ZOOM_MID,
        ZOOM_FAAR
    };
    SphereTones();
    ~SphereTones();
    
    void setup();
    void resetView();
    void clear();
    void update();
    void draw();
    
    void onTouch(ofVec2f p);
    void onUndoDrag();
    void onDrag(ofVec2f p);
    void onRelease(ofVec2f p);
    void onPan(ofVec2f p);
    void onZoom(ofVec2f p, float amount);
    
    Sphere * addSphere(ofVec2f p, int sampleId);
    Sphere * getSphere(ofVec2f p, float sphereRadiusMultiply, float ballRadiusMultiply, float sliderRadiusMultiply);
    void removeSphere(Sphere * sphere);

    float getZoomPercent();
    void updateZoomLevels();
    void updateVolumes();
    void loadTextures();
    void unloadTextures();
    void updateResolution();
    void drawScreenBorder();

    void save();
    void load();
    
    void onSamplePreview(SampleSelectorEventArgs & e);
    void onSampleSelected(SampleSelectorEventArgs & e);
    
    SampleSelector sampleSelector;
    InfoScreen infoScreen;
    ofTexture texMask;

    // spheres
    vector<Sphere*> spheres;
    Sphere * currentSphere;

    // sampler selector vars
    int selectorOpenInterval;
    Timer selectorTimer;
    bool selecting;
    ofVec2f screenTouchPoint, pScreenTouchPoint;
    
    // speed of the level change animation
    float levelChangeSpeed;
    
    // zooming variables
    float viewScaleMin, viewScaleMax;
    float zoomPercentClose, zoomPercentFaar;
    
    // recording
    bool recording;
    ofTexture texRec;
    ofRectangle recView;
    ofEvent<string> audioSavedEvents;

    
    // static functions
    static void drawSphereIn(ofVec2f p, float r);
    static void drawSphereOut(ofVec2f p, float r);
    
    static int getZoomLevel();
    static float getLevel0Percent();
    static float getLevel1Percent();
    static float getLevel2Percent();
    
    static bool isPointOutside(ofVec2f p);
    static bool isSphereOutside(Sphere * sphere);
    
    static ofRectangle getScreenRectInWorldSpace();
    static ofVec2f getScreenPos(ofVec2f worldPos);
    static ofVec2f getClosestPoint(ofVec2f pt1, ofVec2f pt2, ofVec2f p);
    
    static bool isPointOverLine(ofVec2f start, ofVec2f end, float width, ofVec2f p);
    static void drawLine(ofVec2f p1, ofVec2f p2, float width);
    
    static float sq(float a) {return a * a;};

    // static vars
    static ofTexture texSphereOutside;
    static ofTexture texSphereInside;
    
    static TangibleView tView;

    static float sphereRadius, ballRadius;
    static float sliderStartBorder, sliderEndBorder, sphereBorderSize, sliderSize;
    static float grabDistanceMultiply;
    static float ballStep, sliderNumNotes, bounceSpeed;
    static float maxCenterSphereDistance;
    static float maxNumOfSphereElements;
    static float radialTextRadius;
    
    static float screenBorder;
    static float fogBorder;

    static int zoomLevel;
    static float level0Percent;
    static float level1Percent;
    static float level2Percent;

    static float resolutionScale;
    static float mmToPixels;
    
    static float masterVolume;
};

#endif
