#include "InfoScreen.h"
#include "SphereTones.h"

InfoScreen::InfoScreen()
{

}

InfoScreen::~InfoScreen()
{
    ofUnregisterURLNotification(this);
}

void InfoScreen::setup()
{
    loadTextures();

    pSlide = currentSlide = slides.begin();
    slideInterval = 1000 * 5;
    slideStarted = ofGetElapsedTimeMillis();
    currentAlpha = 0;

    damp = 0.95;
    openPct = 1;
    
    float buttonSize = 60 * SphereTones::resolutionScale;
    float buttonOffsetX = -20 * SphereTones::resolutionScale;
    float buttonOffsetY = 15 * SphereTones::resolutionScale;
    button.set(buttonOffsetX, buttonOffsetY, buttonSize, buttonSize);
    
    closing = false;
}

void InfoScreen::loadTextures()
{
    float scale = 0.9;
    for (int i = 0; i < 5; i++)
    {
        ofTexture * img = new ofTexture();
        ofLoadImage(*img, "info_" + ofToString(i) + ".png");
        slides.push_back(img);
        
        if (i == 0)
        {
            float imageRatio = img->getWidth() / (float)img->getHeight();
            height = ofGetHeight() * scale;
            width = height * imageRatio;
        }
    }
    
    ofRegisterURLNotification(this);
    string url = "http://www.binaura.net/bnc/SphereTones/info_online.png";
    ofLoadURLAsync(url);
}

void InfoScreen::unloadTextures()
{
    ofUnregisterURLNotification(this);
    for (int i = 0; i < slides.size(); i++)
    {
        delete slides[i];
    }
    slides.clear();
}

bool InfoScreen::onTouch(ofVec2f p)
{
    if (openPct != 0 && ofRectangle(button.x + ofGetWidth() / 2. - width / 2., button.y, button.width, button.height).inside(p))
    {
        closing = true;
        return true;
    }
    
    pSlide = currentSlide;
    currentSlide++;
    if (currentSlide >= slides.end()) currentSlide = slides.begin();
    slideStarted = ofGetElapsedTimeMillis();
    currentAlpha = 0;
    
    if (openPct > 0) return true;
    return false;
}

void InfoScreen::update()
{
    if (openPct == 0) return;
    if (closing)
    {
        openPct *= damp;
    }
    if (openPct < 0.01) openPct = 0;
    
    if (ofGetElapsedTimeMillis() - slideStarted > slideInterval)
    {
        pSlide = currentSlide;
        currentSlide++;
        if (currentSlide >= slides.end()) currentSlide = slides.begin();
        slideStarted = ofGetElapsedTimeMillis();
        currentAlpha = 0;
    }

    currentAlpha = ofClamp(currentAlpha + 0.05, 0, 1);    
}

void InfoScreen::draw()
{
    if (openPct == 0) return;

    ofSetColor(0, 0, 0, 200 * openPct);
    ofFill();
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2. - width / 2., openPct * height - height);
    
    if (currentAlpha < 1)
    {
        ofSetColor(255, 255 * openPct);
        (*pSlide)->draw(0, 0, width, height);
    }
    ofSetColor(255, 255 * currentAlpha * openPct);
    (*currentSlide)->draw(0, 0, width, height);
    
    drawButton(button);
    
    ofPopMatrix();
}

void InfoScreen::drawButton(ofRectangle view)
{
    float radius = button.getWidth() / 2.;
    ofSetColor(255);
    ofFill();
    ofCircle(button.getCenter(), radius);
    ofSetColor(60);

    float xSc = 0.4;
    SphereTones::drawLine(button.getCenter() + ofVec2f(-radius, -radius) * xSc, button.getCenter() + ofVec2f(radius, radius) * xSc, 5);
    SphereTones::drawLine(button.getCenter() + ofVec2f(-radius, radius) * xSc, button.getCenter() + ofVec2f(radius, -radius) * xSc, 5);
    
    ofSetColor(60, 50);
    ofNoFill();
    ofCircle(button.getCenter(), radius);

}

void InfoScreen::urlResponse(ofHttpResponse &e)
{
    ofLogNotice() << "InfoScreen::urlResponse status: " << e.status;
    ofPixels pix;
    ofLoadImage(pix, e.data);
    if (pix.isAllocated() && slides.size())      // check if the pixels were loaded succesfully
    {
        ofTexture * t = slides[0];
        t->clear();
        t->allocate(pix);
        t->loadData(pix);
        currentSlide = slides.begin();
        slideStarted = ofGetElapsedTimeMillis();
    }
    ofUnregisterURLNotification(this);
}

