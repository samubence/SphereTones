//
//  GestureDetector.cpp
//  SphereTones
//
//  Created by bence samu on 23/10/15.
//
//

#include "GestureDetector.h"


void GestureDetector::setup(GestureListener * _listener) {
    listener = _listener;
    touchA = touchB = NULL;
    dragging = false;
    oTouchDistance = 0;
#ifdef TARGET_ANDROID
    minDragDistance = 10;
#else
    minDragDistance = 2;
#endif
}
;

void GestureDetector::releaseAll()
{
	if (touchA)
	{
		delete touchA;
		touchA = NULL;
	}
	if (touchB)
	{
		delete touchB;
		touchB = NULL;
	}

}
void GestureDetector::draw() {
    float s = 60;
    ofSetColor(0, 255, 255);
    ofNoFill();
    
    if (touchA && !touchB) {
        ofCircle(*touchA, s);
    }
    if (touchA && touchB) {
        ofCircle(*touchA, s);
        ofCircle(touchCenter, 10);
        ofCircle(*touchB, s);
    }
}

void GestureDetector::onTouch(int id, float x, float y)
{
    // if this is the first touch, create touchA
    if (!touchA && !touchB)
    {
        touchA = new GestureTouch(x, y, id);
        pTouchPoint.set(x, y);
        listener->onTouch(ofVec2f(x, y));
    }
    // if this is the secont touch, craete touchB and start zooming
    else if (!touchB && touchA->id != id)
    {
        touchB = new GestureTouch(x, y, id);
        if (dragging)	// if we are drgging, we assume it was a false gesture, start zooming instead
        {
            listener->onUndoDrag();
        }
        else
        {
            listener->onRelease(*touchA);	// finally deliver event
        }
        dragging = false;
    }
    // in any cases, when there are two fingers we start panning
    if (touchA && touchB) {
        oTouchDistance = touchA->distance(*touchB);
        touchCenter = *touchA + (*touchB - *touchA) / 2.f;
        pTouchCenter = touchCenter;
    }
}

void GestureDetector::onDrag(int id, float x, float y)
{
    // update position of touches
    if (touchA) {
        if (touchA->id == id) {
            touchA->setup(x, y, id);
        }
    }
    if (touchB) {
        if (touchB->id == id) {
            touchB->setup(x, y, id);
        }
    }
    
    // when there is only one finger lets drag
    if (touchA && !touchB)
    {
        if (pTouchPoint.distance(*touchA) > minDragDistance || dragging) {
            listener->onDrag(*touchA);
            dragging = true;
        }
    }
    // in case of two fingers PAN and ZOOM
    if (touchA && touchB) {
        touchCenter.set(*touchA + (*touchB - *touchA) / 2.f);
        
        float touchDistance = touchA->distance(*touchB);
        
        //if (touchDistance > minDragDistance)
        {
            listener->onZoom(touchCenter, touchDistance / oTouchDistance);
            oTouchDistance = touchDistance;
        }
        
        listener->onPan(touchCenter - pTouchCenter);
        
        pTouchCenter = touchCenter;
    }
}

void GestureDetector::onRelease(int id, float x, float y)
{
    // when there was only one finger down, release it
    if (touchA && !touchB) {
        if (touchA->id == id) {
            listener->onRelease(ofVec2f(x, y));
            delete touchA;
            touchA = NULL;
            dragging = false;
        }
    }
    // when there were two fingers, update touchA to be the one remaining
    else if (touchA && touchB)
    {
        if (id == touchA->id)
        {
            listener->onRelease(ofVec2f(x, y));
        	delete touchA;
        	touchA = NULL;
        }
        else if (id == touchB->id)
        {
            delete touchB;
            touchB = NULL;
        }
    }
    if (touchB)
    {
    	if (id == touchB->id)
    	{
    		delete touchB;
    		touchB = NULL;
    	}
    }
};
