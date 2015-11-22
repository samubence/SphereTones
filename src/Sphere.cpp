#include "Sphere.h"
#include "SphereTones.h"

Sphere::Sphere(ofVec2f p, int _sampleId)
{
    center = p;
    sampleId = _sampleId;
    
    alive = true;
    dragging = false;
    visible = true;
    setVolume(0);
    creationTimer.start(400);
    draggedElement = NULL;
    newElement = NULL;
    
    calcBBox();
}

Sphere::~Sphere()
{

}

void Sphere::update()
{
    // change visibility based on screen space position
    visible = !SphereTones::isSphereOutside(this);
    
    if (visible)
    {
        for (vector<SphereElement*>::iterator i = elements.begin(); i != elements.end();)
        {
            if ((*i)->alive)
            {
                (*i)->update();
                i++;
            }
            else
            {
                delete *i;
                i = elements.erase(i);
            }
        }
        // to avoid mouse scroll and drag together
        if (SphereTones::getZoomLevel() != 2) dragging = false;
    }
}

void Sphere::draw()
{
    if (visible)
    {
        // grow animtion
        float growPercent = creationTimer.getPercent();

        // the textures has shadows, so we need to scale them to overlap the real circle
        float textureScale = 1.23;

        drawInside(growPercent * textureScale);

        for (vector<SphereElement*>::iterator i = elements.begin(); i != elements.end(); i++)
        {
            (*i)->draw();
        }

        drawOutside(growPercent * textureScale);

        float level2 = SphereTones::getLevel2Percent();
        
        if (level2 > 0)
        {
            ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);

            float maxDst = max(bboxP2.x - bboxP1.x, bboxP2.y - bboxP1.y) * 0.6;
            float maxLineBrightness = 111;
            for (vector<SphereElement*>::iterator i = elements.begin(); i != elements.end(); i++)
            {
                for (vector<SphereElement*>::iterator j = elements.begin(); j != elements.end(); j++)
                {
                    if (i != j)
                    {
                        float d = (*i)->ball.distance((*j)->ball);
                        if (d < maxDst)
                        {
                            float dstNorm = ofMap(d, 0, maxDst, 0, 1, true);
                            float colorMultiply = ofMap(level2, 0, 1, 255, maxLineBrightness + dstNorm * (255 - maxLineBrightness));
                            ofSetColor(colorMultiply);
                            SphereTones::drawLine((*i)->ball, (*j)->ball, 10);
                        }
                    }
                }
                
                float dstNorm = ofMap((*i)->ball.distance(center), 0, maxDst, 0, 1, true);
                float colorMultiply = ofMap(level2, 0, 1, 255, maxLineBrightness + dstNorm * (255 - maxLineBrightness));
                ofSetColor(colorMultiply);
                SphereTones::drawLine(center, (*i)->ball, 10);
            }
            ofEnableAlphaBlending();
        }
    }
}

void Sphere::drawRadialText()
{
	if (draggedElement)
	{
        if (draggedElement->enable)
        {
            draggedElement->drawRadialText();
        }
	}
}

void Sphere::drawLocation()
{
    //if (!visible)
    {
        
        ofRectangle screenView = SphereTones::getScreenRectInWorldSpace();
        ofVec2f locCenter;
        bool left = false;
        bool right = false;
        bool top = false;
        bool bottom = false;
        
        float blockSize = 10 * SphereTones::resolutionScale;
        
        float sr = SphereTones::sphereRadius;
        
        if (center.x < screenView.x)
        {
            left = true;
        }
        else if (center.x > screenView.x + screenView.width)
        {
            right = true;
        }
        
        if (center.y < screenView.y)
        {
            top = true;
        }
        else if (center.y > screenView.y + screenView.height)
        {
            bottom = true;
        }
        
        if (left)
        {
            drawLocationBox(ofRectangle(screenView.x, ofClamp(center.y, screenView.y, screenView.y + screenView.height) - sr, blockSize, sr * 2));
        }
        if (right)
        {
            drawLocationBox(ofRectangle(screenView.x + screenView.width - blockSize, ofClamp(center.y, screenView.y, screenView.y + screenView.height) - sr, blockSize, sr * 2));
        }
        if (top)
        {
            drawLocationBox(ofRectangle(ofClamp(center.x, screenView.x, screenView.x + screenView.width) - sr, screenView.y, sr * 2, blockSize));
        }
        if (bottom)
        {
            drawLocationBox(ofRectangle(ofClamp(center.x, screenView.x, screenView.x + screenView.width) - sr, screenView.y + screenView.height - blockSize, sr * 2, blockSize));
        }
        
    }
}
void Sphere::drawLocationBox(ofRectangle locationView)
{
    ofSetColor(0, 90);
    ofFill();
    ofRect(locationView);
    
    ofSetColor(0, 170);
    ofNoFill();
    ofRect(locationView);
}

void Sphere::drawInside(float scale)
{
    float percent = SphereTones::getLevel0Percent();
    if (percent != 0)
    {
        float r = SphereTones::sphereRadius;
        ofFill();
        ofSetColor(255, 255, 255, percent * 255);
        SphereTones::drawSphereIn(center, r * scale);
    }
}

void Sphere::drawOutside(float scale)
{
    float percent = SphereTones::getLevel0Percent();
    if (percent < 1)
    {
        float r = SphereTones::sphereRadius;
        ofSetColor(255, 255, 255, 255 * (1 - percent));
        SphereTones::drawSphereOut(center, r * scale);
    }
}

void Sphere::setSampleId(int id)
{
    sampleId = id;
    for (vector<SphereElement*>::iterator i = elements.begin(); i != elements.end(); i++)
    {
        (*i)->sampleId = sampleId;
    }
}

void Sphere::move(ofVec2f p)
{
    if (!dragging) dragShift = center - p;
    dragging = true;

    center = p + dragShift;

    for (vector<SphereElement*>::iterator i = elements.begin(); i != elements.end(); i++)
    {
        (*i)->updatePositions(center);
    }

}

void Sphere::onTouch(ofVec2f p)
{
    touchPosition = p;

    int level = SphereTones::getZoomLevel();

    if (level == SphereTones::ZOOM_CLOSE)
    {
        SphereElement * slider = getElementBySlider(p, SphereTones::grabDistanceMultiply);

        if (slider)
        {
            draggedElement = slider;
        }
    }

    if (level == SphereTones::ZOOM_MID)
    {
        SphereElement * ball = getElementByBall(p, SphereTones::grabDistanceMultiply);

        if (ball)
        {
            draggedElement = ball;
        }
    }
}

/*
	we assume that the user accidentally started to drag something when he was to zoom, so we undo the changes
	undo move or delete newly create element if that was the case
 */
void Sphere::onUndoDrag()
{
    if (draggedElement)
    {
    	if (newElement == draggedElement)
    	{
    		newElement->alive = false;
    		newElement = draggedElement = NULL;
    	}
    	else
    	{
    		draggedElement->onUndoDrag();
    	}
    }
}

void Sphere::onDrag(ofVec2f p)
{
    touchPosition = p;

    int level = SphereTones::getZoomLevel();

    if (level == SphereTones::ZOOM_CLOSE)
    {
        if (draggedElement) draggedElement->setSlider(p);
    }
    if (level == SphereTones::ZOOM_MID)
    {
        if (!draggedElement)
        {
            if (elements.size() < SphereTones::maxNumOfSphereElements)
            {
                newElement = addElement(p, ofRandom(0, 1), ofGetElapsedTimeMillis());
                draggedElement = newElement;
            }
        }
        if (draggedElement)
        {
            draggedElement->setBall(p);
        }
    }

    if (level == SphereTones::ZOOM_FAAR)
    {
        move(p);
    }
}

void Sphere::onRelease(ofVec2f p)
{
    touchPosition = p;

    int level = SphereTones::getZoomLevel();

    if (level == SphereTones::ZOOM_MID)
    {
        if (isPointOverCenter(p, 1) && draggedElement)
            draggedElement->alive = false;
    }

    if (level == SphereTones::ZOOM_FAAR)
    {
        if (SphereTones::isPointOutside(p) && dragging) alive = false;
    }

    if (draggedElement) draggedElement->onRelease();
    draggedElement = NULL;
    dragging = false;
    newElement = NULL;

    calcBBox();
}

void Sphere::setVolume(float volume)
{
    for (vector<SphereElement*>::iterator i = elements.begin(); i != elements.end(); i++)
    {
        (*i)->setVolume(volume);
    }
}

SphereElement * Sphere::addElement(ofVec2f p, float sliderPercent, int started)
{
    SphereElement * element = new SphereElement(center, p, sliderPercent, started, sampleId);
    elements.push_back(element);
    return element;
}

bool Sphere::isPointOver(ofVec2f p, float radiusMultiply)
{
    return (isPointOverCenter(p, radiusMultiply) || isPointOverElement(p, radiusMultiply));
}

bool Sphere::isPointOverCenter(ofVec2f p, float radiusMultiply)
{
    return p.distance(center) < SphereTones::sphereRadius * radiusMultiply;
}

bool Sphere::isPointOverElement(ofVec2f p, float radiusMultiply)
{
    if (getElementByBall(p, radiusMultiply)) return true;
    return getElementBySlider(p, radiusMultiply);
}

SphereElement * Sphere::getElementByBall(ofVec2f p, float radiusMultiply)
{
    SphereElement * e = NULL;
    float minDst = SphereTones::ballRadius * radiusMultiply;

    for (vector<SphereElement*>::iterator i = elements.begin(); i != elements.end(); i++)
    {
        ofVec2f cp = SphereTones::getClosestPoint((*i)->ballStart, (*i)->ballEnd, p);
        float d = cp.distance(p);
        if (d < minDst)
        {
            minDst = d;
            e = *i;
        }
    }

    return e;
}

SphereElement * Sphere::getElementBySlider(ofVec2f p, float radiusMultiply)
{
    SphereElement * e = NULL;
    float minDst = SphereTones::ballRadius * radiusMultiply;

    for (vector<SphereElement*>::iterator i = elements.begin(); i != elements.end(); i++)
    {
        //ofVec2f cp = SphereTones::getClosestPoint((*i)->sliderStart, (*i)->sliderEnd, p);
        //float d = cp.distance(p);
        float d = (*i)->slider.distance(p);
        if (d < minDst)
        {
            minDst = d;
            e = *i;
        }
    }

    return e;
}

void Sphere::calcBBox()
{
    float maxRadius = SphereTones::sphereRadius;

    for (int i = 0; i < elements.size(); i++)
    {
        maxRadius = max(maxRadius, elements[i]->length + SphereTones::sphereRadius + SphereTones::ballRadius * 2);
    }

    bboxP1.x = center.x - maxRadius;
    bboxP1.y = center.y - maxRadius;
    bboxP2.x = center.x + maxRadius;
    bboxP2.y = center.y + maxRadius;
}

void Sphere::save(ofXml * xml)
{
    xml->addValue("sampleId", sampleId);
    xml->addValue("centerX", center.x);
    xml->addValue("centerY", center.y);
    
    ofXml xmlElements;
    xmlElements.addChild("elements");
    xmlElements.setTo("elements");
    for (vector<SphereElement*>::iterator i = elements.begin(); i != elements.end(); i++)
    {
        ofXml xmlElement;
        xmlElement.addChild("element");
        xmlElement.setTo("element");
        xmlElement.addValue("started", (*i)->started);
        xmlElement.addValue("ballEndX", (*i)->ballEnd.x);
        xmlElement.addValue("ballEndY", (*i)->ballEnd.y);
        xmlElement.addValue("sliderPercent", (*i)->sliderPercent);
        
        xmlElements.addXml(xmlElement);
        
    }
    xml->addXml(xmlElements);
}

void Sphere::load(ofXml * xml)
{
    xml->setTo("elements");
    int n = xml->getNumChildren("element");
    for (int i = 0; i < n; i++)
    {
        xml->setTo("element[" + ofToString(i) + "]");
        
        int started = xml->getValue<float>("started");
        float ballEndX = xml->getValue<float>("ballEndX");
        float ballEndY = xml->getValue<float>("ballEndY");
        float sliderPercent = xml->getValue<float>("sliderPercent");
        ofVec2f ballEnd = ofVec2f(ballEndX, ballEndY);
        addElement(ballEnd, sliderPercent, started);
        xml->setToParent();
    }
    xml->setToParent();

    calcBBox();
}

