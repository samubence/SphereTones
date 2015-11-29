#include "SphereElement.h"
#include "SphereTones.h"
#include "FontTools.h"
#include "Sampler.h"


SphereElement::SphereElement(ofVec2f _sphereCenter, ofVec2f p, float _sliderPercent, int _started, int _sampleId)
{
    sphereCenter = _sphereCenter;

    sliderPercent = _sliderPercent;
    clampSliderPercent();

    started = _started;
    sampleId = _sampleId;

    /*
     FIX this: setBall only works when SphereTones::getZoomLevel() == 1, we set the default zoom level in SphereTones::SpnereTones()... but it is not nice
     */

    setBall(p);

    volume = 1;
    alive = true;
    pathPositive = false;
    banged = true;

    sliderLength = SphereTones::sphereRadius - (SphereTones::sliderEndBorder + SphereTones::sliderStartBorder);

    enable = true;
    draggingBall = draggingSlider = false;

    bangAnimInterval = 500;
    bangAnimTimer.start(1);

    notes.push_back("C");
    notes.push_back("Db");
    notes.push_back("D");
    notes.push_back("Eb");
    notes.push_back("E");
    notes.push_back("F");
    notes.push_back("Gb");
    notes.push_back("G");
    notes.push_back("Ab");
    notes.push_back("A");
    notes.push_back("Bb");
    notes.push_back("B");
}

SphereElement::~SphereElement()
{

}

void SphereElement::update()
{
    if (!enable) return;

    // if dragging ball should stay in touchPosition
    if (draggingBall) started = ofGetElapsedTimeMillis();

    // calculate bouncing motion
    int delta = ofGetElapsedTimeMillis() - started;
    float a = fmodf(delta * SphereTones::bounceSpeed * (1 / length), TWO_PI);
    if (a < 0) a += TWO_PI;
    pathPercent = cosf(a);

    // detect ball sphere collision
    if ((pathPercent < 0 && pathPositive) || (pathPercent > 0 && !pathPositive))
    {
        pathPositive = pathPositive ? false : true;
        if (!banged)
        {
            banged = true;
            bang();
        }
    }
    else banged = false;

    pathPercent = fabsf(pathPercent);

    // update ball position
    float d = ofDist(ballStart.x, ballStart.y, ballEnd.x, ballEnd.y) * pathPercent;
    ball.x = ballStart.x + cosf(angle) * d;
    ball.y = ballStart.y + sinf(angle) * d;
}

void SphereElement::bang()
{
    float frequency = getFrequency(note);
    float freqScale = 21.8519 * 2.;//ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 50); cout << freqScale << endl;
    float speed = frequency / freqScale;
    // magic number here

    Sampler::play(sampleId, volume * SphereTones::masterVolume, speed);

    bangAnimTimer.start(bangAnimInterval);
}

float SphereElement::getFrequency(int note)
{
    return (440 / 32.f) * (powf(2, ((note - 9) / 12.f)));
}

float SphereElement::getQuanted(float value, float step)
{
    return ((int)(value / step)) * step;
}

void SphereElement::draw()
{
    if (!enable) return;
    float bangAnimPercent = bangAnimTimer.getPercent();
    float ballRadius =  SphereTones::ballRadius * (1 + (1 - bangAnimPercent) * 0.3);
    float textureScale = 1.23;
    
    /// LEVEL INSIDE
    if (SphereTones::getLevel0Percent() > 0)
    {

        // draw line
        ofFill();
        ofSetColor(0, 0, 0, 200 * SphereTones::getLevel0Percent());
        SphereTones::drawLine(sliderEnd, ball, 6);

        // draw ball
        ofSetColor(255, 255, 255, 255 * SphereTones::getLevel0Percent());
        SphereTones::drawSphereOut(ball, ballRadius * textureScale);

        // draw slider path
        ofNoFill();
        ofSetColor(255, 255, 255, 100 * SphereTones::getLevel0Percent());
        SphereTones::drawLine(sliderStart, sliderEnd, SphereTones::sliderSize);

        // draw string anim
        ofFill();
        ofSetColor(255, 0, 0, 255);
        SphereTones::drawLine(slider, sliderEnd, SphereTones::sliderSize * (1 - bangAnimPercent));

        // draw helper circle
        ofNoFill();
        float a = ofMap(bangAnimPercent, 0, 1, 255, 60, true) * SphereTones::getLevel0Percent();
        ofSetColor(255, 255, 255, a);
        ofCircle(sphereCenter, ofMap(getQuanted(sliderPercent, 1 / SphereTones::sliderNumNotes), 0, 1, SphereTones::sliderStartBorder, SphereTones::sphereRadius - SphereTones::sliderEndBorder));
        
       
        // draw slider handle
        ofPushMatrix();

            ofSetColor(255, 255, 255, 255 * SphereTones::getLevel0Percent());
            ofTranslate(slider.x, slider.y, 0);
            glRotatef(ofRadToDeg(angle) + 90, 0, 0, 1);
            ofFill();
            ofRect(-SphereTones::sliderSize / 2., - SphereTones::sliderSize / 2., SphereTones::sliderSize, SphereTones::sliderSize);
        ofPopMatrix();

        //string text = ofToString(notes[note % notes.size()]);ofDrawBitmapStringHighlight(text, slider);

    }

    /// LEVEL RYTHM
    if (SphereTones::getLevel1Percent() > 0)
    {
        // draw line
        ofFill();
        ofSetColor(0, 0, 0, 200 * SphereTones::getLevel1Percent());
        SphereTones::drawLine(sliderEnd, ball, 6);

        // draw path
        ofFill();
        ofSetColor(0, 0, 0, 128 * SphereTones::getLevel1Percent());
        SphereTones::drawLine(sliderEnd, ballEnd, 2);

        // helper circle
        float a = ofMap(bangAnimPercent, 0, 1, 90, 30, true);
        ofNoFill();
        ofSetColor(0, 0, 0, a * SphereTones::getLevel1Percent());
        ofCircle(sphereCenter, SphereTones::sphereRadius + length);

        // draw ball

        ofSetColor(255, 255, 255, 255 * SphereTones::getLevel1Percent());
        SphereTones::drawSphereOut(ball, ballRadius * textureScale);

        // draw text
        ofPushMatrix();
            ofSetColor(255, 0, 0, 166 * SphereTones::getLevel1Percent());
            ofFill();

            ofTranslate(ballEnd.x, ballEnd.y, 0);
            ofRotate(ofRadToDeg(angle) + 90);

            string txt = ofToString((int)(length / SphereTones::ballStep));
            if (!draggingBall)
            {
            	float fontScale = 1 * SphereTones::resolutionScale;
                ofTranslate(0, -40, 0);
                FontTools::drawText(txt);
            }

        ofPopMatrix();
    }

    /// LEVEL NODES
    if (SphereTones::getLevel2Percent() > 0)
    {
        // draw rhythm circles
        ofNoFill();
        ofSetColor(0, 0, 0, 255 * SphereTones::getLevel2Percent() * (1 - bangAnimPercent));
        ofCircle(sphereCenter, SphereTones::sphereRadius * 1.1 * (1 + sliderPercent));
        
        // draw ball
        ofFill();
        ofSetColor(255, 255, 255, 255 * SphereTones::getLevel2Percent());
    }
}

void SphereElement::drawRadialText()
{
    string txt = "";
    ofVec2f p;
    int numOfTexts = 7;
    
	if (draggingBall)
	{
        txt = ofToString((int)(length / SphereTones::ballStep));
        ofSetColor(255, 0, 0, 166 * SphereTones::getLevel1Percent());
        p = SphereTones::getScreenPos(ballEnd);
    }
    else if (draggingSlider)
    {
        txt = notes[note % notes.size()];
        ofSetColor(255, 0, 0, 166 * SphereTones::getLevel0Percent());
        p = SphereTones::getScreenPos(slider);
        numOfTexts = 4;
    }
    
    float angleStep = 360 / (float)numOfTexts;
    float distance = SphereTones::radialTextRadius * SphereTones::mmToPixels;
    float fontScale = 0.3 * SphereTones::resolutionScale;
    
    ofFill();
    ofPushMatrix();
    ofTranslate(p);
    
    for (int i = 0; i < numOfTexts; i ++)
    {
        glRotatef(angleStep, 0, 0, 1);
        ofPushMatrix();
        ofTranslate(distance, 0, 0);
        ofScale(fontScale, fontScale);
        FontTools::drawText(txt);
        ofPopMatrix();
    }
    ofPopMatrix();

}

/*
    update parent's SphereCenter and calculates ball and slider positions when Sphere is moving
*/
void SphereElement::updatePositions(ofVec2f _sphereCenter)
{
    sphereCenter = _sphereCenter;

    ofVec2f v = ofVec2f(cosf(angle), sinf(angle));
    ballStart = sphereCenter + v * (SphereTones::sphereRadius + SphereTones::ballRadius);
    ballEnd = ballStart + v * (length - SphereTones::ballRadius);
    sliderStart = sphereCenter + v * SphereTones::sliderStartBorder;
    sliderEnd = sphereCenter + v * (SphereTones::sphereRadius - SphereTones::sliderEndBorder);

    slider = sliderStart + (sliderEnd - sliderStart) * sliderPercent;

}

/*
    forces ball path's length in a certain resolution
*/
void SphereElement::clampLength()
{
    length = (int)((max(SphereTones::ballStep / 2.f, length) + SphereTones::ballStep / 2.f) / SphereTones::ballStep) * SphereTones::ballStep;
    length = max(SphereTones::ballStep * 2, length);
}

/*
    forces sliderPercent in a certain resolution
*/
void SphereElement::clampSliderPercent()
{
    note = (int)(sliderPercent * SphereTones::sliderNumNotes);
}

/*
    sets ball position and calculates:
        - angle
        - langth
*/
void SphereElement::setBall(ofVec2f p)
{
    if (SphereTones::getZoomLevel() != 1) return;
    
    if (!draggingBall)
    {
        pAngle = angle;
        pLength = length;
        pStarted = started;
    }
    draggingBall = true;

    enable = sphereCenter.distance(p) > SphereTones::sphereRadius;

    angle = atan2(p.y - sphereCenter.y, p.x - sphereCenter.x);

    length = sphereCenter.distance(p) - SphereTones::sphereRadius;

    clampLength();

    updatePositions(sphereCenter);

    banged = true;
}

/*
    stets slider position
*/
void SphereElement::setSlider(ofVec2f p)
{
    if (SphereTones::getZoomLevel() != 0) return;

    if (!draggingSlider)
    {
        pSliderPercent = sliderPercent;
    }
    draggingSlider = true;

    float startDst = sliderStart.distance(p);
    float endDst = sliderEnd.distance(p);

    if (startDst >= sliderLength && endDst < startDst)
    {
        sliderPercent = 1;
    }
    else if (endDst >= sliderLength && startDst < endDst)
    {
        sliderPercent = 0;
    }
    else sliderPercent = startDst / sliderLength;

    clampSliderPercent();

    slider = sliderStart + (sliderEnd - sliderStart) * sliderPercent;
}


void SphereElement::onUndoDrag()
{
    if (draggingBall)
    {
        angle = pAngle;
        length = pLength;
        started = pStarted;
        draggingBall = false;
        enable = true;
        updatePositions(sphereCenter);
    }
    if (draggingSlider)
    {
        draggingSlider = false;
        sliderPercent = pSliderPercent;
        updatePositions(sphereCenter);
    }
}

void SphereElement::onRelease()
{
    draggingBall = draggingSlider = false;
}

void SphereElement::setVolume(float _volume)
{
    volume = _volume;
}
