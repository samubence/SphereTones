//
//  SampleSelector.cpp
//  SphereTones
//
//  Created by bence samu on 20/11/15.
//
//

#include "SampleSelector.h"
#include "SphereTones.h"

SampleSelector::SampleSelector()
{
    
    alive = false;
}

void SampleSelector::setup(int _idMax)
{
    idMax = _idMax;
    overId = pOverId = -1;
    radius = 17 * SphereTones::mmToPixels;
    minRadius = 3 * SphereTones::mmToPixels;
    alive = false;
}

void SampleSelector::update(ofVec2f screenTouchPoint, float _percent)
{
    touchPoint = screenTouchPoint;
    percent = _percent;
    
    if (percent == 1)
    {
        overId = getElementOverId(touchPoint);
        
        if (overId != pOverId && overId != -1 && alive)
        {
            SampleSelectorEventArgs e;
            e.id = overId;
            ofNotifyEvent(onSamplePreview, e);
        }
    }
    else
    {
        overId = -1;
        float d = center.distance(touchPoint);
        if (d > minRadius)
        {
            alive = false;
            percent = 0;
        }
    }
    
    pOverId = overId;
}

void SampleSelector::draw(bool inverz)
{
    if (percent < 0.2 || !alive) return;
    
    ofSetCircleResolution(64);
    
    ofSetColor(inverz ? 60 : 255, 128 * percent);
    ofFill();
    ofCircle(center, radius * percent);
    
    ofSetColor(inverz ? 255 : 0, 255 * percent);
    ofNoFill();
    ofCircle(center, radius * percent);
    
    
    float angleStep = TWO_PI / (float)idMax;
    
    for (int i = 0; i < idMax; i++)
    {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;
        
        if (i == overId)
        {
            ofSetColor(255, 0, 0, 155 * percent);
            ofFill();
            arc(center, a1, a2, 0, radius * percent, 64);
        }
        
        ofSetColor(inverz ? 255 : 0, 255 * percent);
        ofLine(center.x + cosf(a1) * minRadius * percent, center.y + sinf(a1) * minRadius * percent, center.x + cosf(a1) * radius * percent, center.y + sinf(a1) * radius * percent);
    }
    
    ofSetColor(255, 255 * percent);
    ofFill();
    ofCircle(center, minRadius * percent);
    ofSetColor(inverz ? 255 : 0, 255 * percent);
    ofNoFill();
    ofCircle(center, minRadius * percent);
}

void SampleSelector::open(ofVec2f p)
{
    center = p;
    touchPoint = p;
    alive = true;
    overId = -1;
}

void SampleSelector::onRelease()
{
    if (alive && overId != -1)
    {
        SampleSelectorEventArgs e;
        e.id = overId;
        e.position = center;
        ofNotifyEvent(onSampleSelected, e);
    }
    percent = 0;
}

int SampleSelector::getElementOverId(ofVec2f p)
{
    float d = p.distance(center);
    if (d < minRadius || d > radius) return -1;
    float a = ofRadToDeg(atan2f(p.y - center.y, p.x - center.x));
    if (a < 0) a += 360;
    return (int)ofMap(a, 0, 360, 0, idMax);
}
