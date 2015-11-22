#include "SphereTones.h"
#include "FontTools.h"
#include "Sampler.h"

//
TangibleView SphereTones::tView;
ofTrueTypeFont FontTools::font;
ofTexture SphereTones::texSphereOutside;
ofTexture SphereTones::texSphereInside;

//  //
int SphereTones::zoomLevel;
float SphereTones::level0Percent;
float SphereTones::level1Percent;
float SphereTones::level2Percent;

//  //  //
float SphereTones::maxCenterSphereDistance;
float SphereTones::screenBorder;
float SphereTones::fogBorder;
float SphereTones::resolutionScale;

//  //  defaults
float SphereTones::sphereRadius = 300;
float SphereTones::ballRadius = 50;
float SphereTones::sliderStartBorder = 20;
float SphereTones::sliderEndBorder = 15;
float SphereTones::sphereBorderSize;
float SphereTones::grabDistanceMultiply = 2.5;
float SphereTones::ballStep = 40;
float SphereTones::sliderNumNotes = 12 * 3;
float SphereTones::bounceSpeed = 0.8;
float SphereTones::sliderSize = 15;
float SphereTones::mmToPixels = 96 / 24.5f;    // monitor DPI converter;
float SphereTones::radialTextRadius = 8; // mm
float SphereTones::maxNumOfSphereElements = 23;

float SphereTones::masterVolume = 1;

SphereTones::SphereTones()
{
    //ofLogNotice() << "SphereTones::SphereTones";

    levelChangeSpeed = 0.03;
    selectorOpenInterval = 500;
    selecting = false;
    zoomLevel = 1;
}

SphereTones::~SphereTones()
{
	Sampler::stopRecording();
}

void SphereTones::resetView()
{
    //float s = 100000;    // a very large space
    tView.setup(viewScaleMin, viewScaleMax);//, ofRectangle(-s / 2., -s / 2., s, s));
    tView.zoom(ofVec2f(ofGetWidth() / 2., ofGetHeight() / 2.), 0.3);
}

void SphereTones::clear()
{
    for (vector<Sphere*>::iterator i = spheres.begin(); i != spheres.end(); i++)
    {
        (*i)->alive = false;
    }
}

void SphereTones::setup()
{
    ofLogNotice() << "SphereTones::setup";
    
    // set resolution depentent vars
    updateResolution();
    
    // initialization

    sampleSelector.setup(Sampler::getNumOfSamples());
    ofAddListener(sampleSelector.onSamplePreview, this, &SphereTones::onSamplePreview);
    ofAddListener(sampleSelector.onSampleSelected, this, &SphereTones::onSampleSelected);

    loadTextures();
    
    infoScreen.setup();
    
    // load saved scene
    resetView();
    load();
    
    recording = false;
    
    if (spheres.size() == 0) resetView();
    
    currentSphere = NULL;
}

void SphereTones::updateResolution()
{
	// ugly hack: dev screen resolution was 1080/2 x 1920/2
    // we should implement a camera based navigation... huge refacor :(
	resolutionScale = ofGetHeight() / (1920 / 2.);

	viewScaleMin = 0.1 * resolutionScale;
	viewScaleMax = 2 * resolutionScale;
	zoomPercentClose = 0.48 * (1 / resolutionScale);
	zoomPercentFaar =  0.01 * (1 / resolutionScale);
	screenBorder = 30 * resolutionScale;
	fogBorder = 100 * resolutionScale;
    
    maxCenterSphereDistance = max(ofGetWidth(), ofGetHeight()) * 0.6;   // this is for controlling overall volume based on screen coordinates
}

void SphereTones::update()
{
    tView.update();

    updateZoomLevels();
    updateVolumes();
    
#ifdef TARGET_ANDROID
    ofSetCircleResolution((int)ofMap(getZoomPercent(), 0, 1, 30, 64, true));
#else
    ofSetCircleResolution((int)ofMap(getZoomPercent(), 0, 1, 64, 128, true));
#endif
    for (vector<Sphere*>::iterator i = spheres.begin(); i != spheres.end();)
    {
        if ((*i)->alive)
        {
            (*i)->update();
            i++;
        }
        else
        {
            delete *i;
            i = spheres.erase(i);
        }
    }

    if (selecting)
    {
        sampleSelector.update(screenTouchPoint, selectorTimer.getPercent());
    }
    // volume goes down when selecting a sound
    masterVolume = ofMap(sampleSelector.percent, 0, 1, 1, 0.2);

    infoScreen.update();
    
    Sampler::updateFilters(getLevel0Percent(), getLevel1Percent(), getLevel2Percent());
}

void SphereTones::draw()
{
    int b = (int)ofMap(getLevel0Percent(), 1, 0, 80, 255, true);
    ofBackground(b, b, b);

    // draw spheres
    tView.begin();

        for (vector<Sphere*>::iterator i = spheres.begin(); i != spheres.end(); i++)
        {
            (*i)->draw();
        }

    tView.end();
    
    // draw fog
    float fogPct = getLevel2Percent();
    if (fogPct != 0)
    {
        int b = (int)ofMap(fogPct, 0, 1, 0, 255, true);
        ofSetColor(b);
        ofEnableBlendMode(OF_BLENDMODE_SCREEN);
        texMask.draw(0, 0, ofGetWidth(), ofGetHeight());
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    }
    
    // draw spheres location indicator
    tView.begin();
    
    for (vector<Sphere*>::iterator i = spheres.begin(); i != spheres.end(); i++)
    {
        (*i)->drawLocation();
    }
    
    tView.end();

    // draw radial text on dragged element in screen space
    for (vector<Sphere*>::iterator i = spheres.begin(); i != spheres.end(); i++)
	{
		(*i)->drawRadialText();
	}

    drawScreenBorder();

    if (selecting)
    {
        sampleSelector.draw(currentSphere != NULL);
    }

    if (recording)
    {
        ofSetColor(255, 0, 0, ofMap(sinf(ofGetElapsedTimef() * 10), -1, 1, 100, 255));
        texRec.draw(recView);
    }
    else
    {
        ofSetColor(200);
        texRec.draw(recView);
    }
  
    infoScreen.draw();

}

/*
    Add a new Sphere object
*/
Sphere * SphereTones::addSphere(ofVec2f p, int sampleId)
{
    Sphere * sphere = new Sphere(p, sampleId);
    spheres.push_back(sphere);
    return sphere;
}

/*
    Returns Sphere under point
    first search for closest sphere Sphere
    if not found, search for Sphere elements
*/
Sphere * SphereTones::getSphere(ofVec2f p, float sphereRadiusMultiply, float ballRadiusMultiply, float sliderRadiusMultiply)
{
    Sphere * s = NULL;
    float minDst = sphereRadius * sphereRadiusMultiply;
    for (vector<Sphere*>::iterator i = spheres.begin(); i != spheres.end(); i++)
    {
        float d = (*i)->center.distance(p);
        if (d < minDst)
        {
            minDst = d;
            s = *i;
        }
    }
    if (s != NULL) return s;

    for (vector<Sphere*>::iterator i = spheres.begin(); i != spheres.end(); i++)
    {
        if ((*i)->getElementByBall(p, ballRadiusMultiply) || (*i)->getElementBySlider(p, sliderRadiusMultiply)) return (*i);
    }

    return NULL;

}

/*
    Removes Sphere
*/
void SphereTones::removeSphere(Sphere * sphere)
{
    for (vector<Sphere*>::iterator i = spheres.begin(); i != spheres.end(); )
    {
        if (*i == sphere)
        {
            delete *i;
            i = spheres.erase(i);
        }
        else i++;
    }
}

/*
    Calculates Sphere distances from screen center on screen coordinates
*/
void SphereTones::updateVolumes()
{
    for (vector<Sphere*>::iterator i = spheres.begin(); i != spheres.end(); i++)
    {
        ofVec2f screenCenter = ofVec2f(ofGetWidth() / 2., ofGetHeight() / 2.);
        ofVec2f sphereCenter = tView.localToScreen((*i)->center);

        float v = ofMap(screenCenter.distance(sphereCenter), 0, maxCenterSphereDistance, 1, 0, true);

        (*i)->setVolume(v);
    }
}

/*
    Update zoom levels, so all class can simply access these variables:
    - getLevel0Percent();
    - getLevel1Percent();
    - getLevel2Percent();
*/
void SphereTones::updateZoomLevels()
{
    float zoomPercent = getZoomPercent();
    zoomLevel = (zoomPercent > zoomPercentClose ? ZOOM_CLOSE : (zoomPercent < zoomPercentFaar ? ZOOM_FAAR : ZOOM_MID));

    float currentChangeSpeed = levelChangeSpeed * (60 / ofGetFrameRate());

    if (zoomLevel == ZOOM_CLOSE)
    {
        level0Percent = ofClamp(level0Percent + currentChangeSpeed, 0, 1);
        level1Percent = ofClamp(level1Percent - currentChangeSpeed, 0, 1);
        level2Percent = ofClamp(level2Percent - currentChangeSpeed, 0, 1);
    }
    else if (zoomLevel == ZOOM_MID)
    {
        level0Percent = ofClamp(level0Percent - currentChangeSpeed, 0, 1);
        level1Percent = ofClamp(level1Percent + currentChangeSpeed, 0, 1);
        level2Percent = ofClamp(level2Percent - currentChangeSpeed, 0, 1);
    }
    else if (zoomLevel == ZOOM_FAAR)
    {
        level0Percent = ofClamp(level0Percent - currentChangeSpeed, 0, 1);
        level1Percent = ofClamp(level1Percent - currentChangeSpeed, 0, 1);
        level2Percent = ofClamp(level2Percent + currentChangeSpeed, 0, 1);
    }
}

/*
    Draws screen border in LEVEL 2
*/
void SphereTones::drawScreenBorder()
{
    float p = SphereTones::getLevel2Percent();
    if (p == 0) return;

    float s = currentSphere ? ofMap(sin(ofGetElapsedTimeMillis() * 0.01), -1, 1, 0, 1, true) : 0;
    ofSetColor(0, 0, 0, p * 30 * s);
    ofFill();
    ofRect(0, 0, ofGetWidth(), screenBorder);
    ofRect(0, screenBorder, screenBorder, ofGetHeight() - screenBorder * 2);
    ofRect(ofGetWidth() - screenBorder, screenBorder, screenBorder, ofGetHeight() - screenBorder * 2);
    ofRect(0, ofGetHeight() - screenBorder, ofGetWidth(), screenBorder);


    ofSetColor(0, 0, 0, p * 255);
    ofNoFill();
    ofRect(screenBorder, screenBorder, ofGetWidth() - screenBorder * 2, ofGetHeight() - screenBorder * 2);
}

/*
    Touch event:
        get underlying Sphere
        if no Sphere found and in LEVEL 1 --> start SampleSelector animation
*/
void SphereTones::onTouch(ofVec2f p)
{
    screenTouchPoint = p;
    pScreenTouchPoint = screenTouchPoint;
    //
    // INFO SCREEN
    //
    if (!infoScreen.onTouch(screenTouchPoint))
    {
        // SPHERES
        p = tView.screenToLocal(p);
        int level = getZoomLevel();
        
        if (level == ZOOM_CLOSE)
        {
            currentSphere = getSphere(p, 1, 0, grabDistanceMultiply);
        }
        else if (level == ZOOM_MID)
        {
            currentSphere = getSphere(p, 1, grabDistanceMultiply, 0);
        }
        else if (level == ZOOM_FAAR)
        {
            currentSphere = getSphere(p, grabDistanceMultiply, 0, 0);
        }
        
        if (currentSphere != NULL)
        {
            currentSphere->onTouch(p);
        }
        
        if (recView.inside(screenTouchPoint))
        {
            //
            // RECORDING
            //
            if (recording)
            {
                Sampler::stopRecording();
                string e = Sampler::getFileName();
                ofNotifyEvent(audioSavedEvents, e);
            }
            else
            {
                Sampler::startRecording("SphereTones." + ofGetTimestampString() + ".wav");
            }
            recording ^= true;
        }
        else
		{
			//
			// SAMPLE SELECTOR
			//
			selecting = true;
			selectorTimer.start(selectorOpenInterval);
			sampleSelector.open(screenTouchPoint);
		}
    }
}
void SphereTones::onUndoDrag()
{
    if (currentSphere) currentSphere->onUndoDrag();
}

/*
    Drag event:
        if selecting in SampleSelector ignore this
        else
        if currentSphere exists fire onDrag event
        else
        pan view
*/
void SphereTones::onDrag(ofVec2f p)
{
    screenTouchPoint = p;
    selecting = sampleSelector.alive;
    
    if (selecting) return;
    
    p = tView.screenToLocal(p);
    if (currentSphere) currentSphere->onDrag(p);
    else
    {
        ofVec2f v = screenTouchPoint - pScreenTouchPoint;
        tView.pan(v);
    }
    
    pScreenTouchPoint = screenTouchPoint;
}

/*
    Release event:
        if currentSphere exists fire onRelease event
        else if in SampleSelector menu fire onRelease in SampleSelector, stop selector timer
*/
void SphereTones::onRelease(ofVec2f p)
{
    screenTouchPoint = p;
    p = tView.screenToLocal(p);
    if (currentSphere) currentSphere->onRelease(p);
    //else
    if (selecting)
    {
        selecting = false;
        sampleSelector.onRelease();
        selectorTimer.start(0);
    }
    currentSphere = NULL;
}

/*
    Navigation events
*/
void SphereTones::onPan(ofVec2f p)
{
    tView.pan(p);
}

void SphereTones::onZoom(ofVec2f p, float amount)
{
    tView.zoom(p, amount);
}

/*
    SampleSelector events
*/
void SphereTones::onSamplePreview(SampleSelectorEventArgs & e)
{
    Sampler::play(e.id, 1, 1);
}

void SphereTones::onSampleSelected(SampleSelectorEventArgs & e)
{
    if (currentSphere)
    {
        currentSphere->setSampleId(e.id);
    }
    else
    {
        addSphere(tView.screenToLocal(e.position), e.id);
    }
}


/*
    Returs current zoom percent
*/
float SphereTones::getZoomPercent()
{
   return ofMap(tView.sc, viewScaleMin, viewScaleMax, 0, 1, true);
}


/// STATIC functions for all classes to use

void SphereTones::drawSphereIn(ofVec2f p, float r)
{
    texSphereInside.draw(p.x - r, p.y - r, r * 2, r * 2);
 }

void SphereTones::drawSphereOut(ofVec2f p, float r)
{
    texSphereOutside.draw(p.x - r, p.y - r, r * 2, r * 2);
}


/*
    calculates if local point is outside of the screen border
*/
bool SphereTones::isPointOutside(ofVec2f p)
{
    p = tView.localToScreen(p);
    return p.x < screenBorder || p.x > ofGetWidth() - screenBorder || p.y < screenBorder || p.y > ofGetHeight() - screenBorder;
}

/*
    calculates if Sphere bounding box is outside the screen
*/
bool SphereTones::isSphereOutside(Sphere * sphere)
{
    ofVec2f b1 = tView.localToScreen(sphere->bboxP1);
    ofVec2f b2 = tView.localToScreen(sphere->bboxP2);

    return (b1.x > ofGetWidth() || b1.y > ofGetHeight() || b2.x < 0 || b2.y < 0);
}

ofRectangle SphereTones::getScreenRectInWorldSpace()
{
    ofVec2f tl = tView.screenToLocal(ofVec2f(0, 0));
    ofVec2f br = tView.screenToLocal(ofVec2f(ofGetWidth(), ofGetHeight()));
    float x = tl.x;
    float y = tl.y;
    float w = br.x - tl.x;
    float h = br.y - tl.y;
    return ofRectangle(x, y, w, h);
}

ofVec2f SphereTones::getScreenPos(ofVec2f worldPos)
{
    return tView.localToScreen(worldPos);
}

/*
    returns current zoom level
*/
int SphereTones::getZoomLevel()
{
    return zoomLevel;
}

/*
    returns level animation percents for all 3 levels
*/
float SphereTones::getLevel0Percent()
{
    return level0Percent;
}

float SphereTones::getLevel1Percent()
{
    return level1Percent;
}

float SphereTones::getLevel2Percent()
{
    return level2Percent;
}

/*
    2D Math functions
*/
ofVec2f SphereTones::getClosestPoint(ofVec2f pt1, ofVec2f pt2, ofVec2f p)
{
        float u = ((p.x - pt1.x) * (pt2.x - pt1.x) + (p.y - pt1.y) * (pt2.y - pt1.y)) / (sq(pt2.x - pt1.x) + sq(pt2.y - pt1.y));
    if (u > 1.0)
      return pt2;
    else if (u <= 0.0)
      return pt1;
    else
      return ofVec2f((int)(pt2.x * u + pt1.x * (1.0 - u) + 0.5), (int)(pt2.y * u + pt1.y * (1.0 - u) + 0.5));
}

bool SphereTones::isPointOverLine(ofVec2f start, ofVec2f end, float width, ofVec2f p)
{
    float ra = atan2(end.y - start.y, end.x - start.x);
    float rd = ofDist(start.x, start.y, end.x, end.y);
    float a = atan2(p.y - start.y, p.x - start.x);
    float d = ofDist(p.x, p.y, start.x, start.y);
    float _x = start.x + cos(a - ra) * d;
    float _y = start.y + sin(a - ra) * d;
    return (_x > start.x && _x < start.x + rd && _y > start.y - width / 2. && _y < start.y + width / 2.);
}

/*
    draws a line with width
*/
void SphereTones::drawLine(ofVec2f p1, ofVec2f p2, float width)
{
    ofPushMatrix();
        ofTranslate(p1.x, p1.y, 0);
        ofRotate(ofRadToDeg(atan2(p2.y - p1.y, p2.x - p1.x)));
        ofRect(0, -width / 2., p1.distance(p2), width);
    ofPopMatrix();
}

/*
    IO functions
*/

void SphereTones::save()
{
    float version = 1.3;
	ofLogNotice() << "SphereTones::save()";
    string fileName = "sphereTones.dat";
    ofXml xmlSphereTones;
    xmlSphereTones.addChild("SphereTones");
    xmlSphereTones.setTo("SphereTones");
    xmlSphereTones.addValue("version", version);
    ofXml xmlTView;
    xmlTView.addChild("tView");
    xmlTView.setTo("tView");
    xmlTView.addValue("centerx", tView.transformPoints.p1.x);
    xmlTView.addValue("centery", tView.transformPoints.p1.y);
    xmlTView.addValue("leftx", tView.transformPoints.p2.x);
    xmlTView.addValue("lefty", tView.transformPoints.p2.y);
    xmlSphereTones.addXml(xmlTView);
    
    ofXml xmlSpheres;
    xmlSpheres.addChild("spheres");
    xmlSpheres.setTo("spheres");
    
    for (vector<Sphere*>::iterator i = spheres.begin(); i != spheres.end(); i++)
    {
        ofXml xmlSphere;
        xmlSphere.addChild("sphere");
        xmlSphere.setTo("sphere");
        (*i)->save(&xmlSphere);
        xmlSpheres.addXml(xmlSphere);
    }
    
    xmlSphereTones.addXml(xmlSpheres);
    
    xmlSphereTones.save(fileName);
}

void SphereTones::load()
{
	ofLogNotice() << "SphereTones::load()";
    string fileName = "sphereTones.dat";
    ofXml xml;
    if (xml.load(fileName))
    {
        xml.setTo("SphereTones");

        xml.setTo("tView");

        float cx = xml.getValue<float>("centerx");
        float cy = xml.getValue<float>("centery");
        float lx = xml.getValue<float>("leftx");
        float ly = xml.getValue<float>("lefty");
        
        tView.transformPoints.p1.set(cx, cy);
        tView.transformPoints.p2.set(lx, ly);
        tView.calcTranstormation();
        tView.setSmooth(0); // change transformation instantly
        tView.update();
        tView.setSmooth();

        xml.setToParent();
        
        xml.setTo("spheres");
        int n = xml.getNumChildren("sphere");
        for (int i = 0; i < n; i++)
        {
            xml.setTo("sphere[" + ofToString(i)+ "]");
            
            int sampleId = xml.getValue<float>("sampleId");
            float centerX = xml.getValue<float>("centerX");
            float centerY = xml.getValue<float>("centerY");
            
            Sphere * s = addSphere(ofVec2f(centerX, centerY), sampleId);
            s->load(&xml);
            
            xml.setToParent();
        }
        xml.setToParent();
    }
}

void SphereTones::loadTextures()
{
	updateResolution();

    ofImage tmp;
    ofLoadImage(texSphereOutside, "sphereOut.png");
    ofLoadImage(texSphereInside, "sphereIn.png");
    
    ofLoadImage(texRec, "rec.png");
    float s = 60 * resolutionScale;
    recView = ofRectangle(ofGetWidth() - s, 0, s, s);

    int downsample = 2;
    int res = ofGetHeight() / downsample;
    
    unsigned char * pixels = new unsigned char[res * res];
    int n = 0;
    for (int y = 0; y < res; y++)
        for (int x = 0; x < res; x++, n++)
        {
            float d = ofDist(x, y, res / 2., res / 2.);
            float p = ofMap(d, 0, res * 0.5, 0, 1, true);
            p = powf(p, 1.3);
            int b = (int)(p * 255);
            pixels[n] = b;
        }
    texMask.allocate(res, res, GL_LUMINANCE);
    texMask.loadData(pixels, res, res, GL_LUMINANCE);
    delete pixels;
}

void SphereTones::unloadTextures()
{
    texSphereInside.clear();
    texSphereOutside.clear();
    texMask.clear();
    texRec.clear();
}

