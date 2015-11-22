#ifndef __GestureDetector_h__
#define __GestureDetector_h__

#include "ofMain.h"

class GestureListener {
public:
	virtual void onTouch(ofVec2f p) {}
	virtual void onDrag(ofVec2f p) {}
	virtual void onRelease(ofVec2f p) {}
	virtual void onPan(ofVec2f p) {}
	virtual void onZoom(ofVec2f p, float amount) {}
	virtual void onUndoDrag(){}
};

/*
 	 Custom two finger gesture detector.
 */

class GestureTouch : public ofVec2f
{
public:
    GestureTouch(float x, float y, int _id) : ofVec2f(x, y)
	{
        id = _id;
	}
	void setup(float x, float y, int _id)
	{
		set(x, y);
		id = _id;
	}

	int id;
};

class GestureDetector {
public:

    void setup(GestureListener * _listener);
    void draw();
    void onTouch(int id, float x, float y);
    void onDrag(int id, float x, float y);
    void onRelease(int id, float x, float y);
    void releaseAll();
    
	GestureListener * listener;
	GestureTouch * touchA, *touchB;
	ofVec2f touchCenter, pTouchCenter;
	float oTouchDistance;
	float minDragDistance;
	ofVec2f pTouchPoint;
	bool dragging;

};

#endif

