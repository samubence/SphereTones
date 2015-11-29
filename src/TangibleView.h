#ifndef TANGIBLEVIEW_H
#define TANGIBLEVIEW_H

class TangibleView
{
public:

    void setup(float _scaleMin = 0.1, float _scaleMax = 5, ofRectangle _bbox = ofRectangle())
    {
        // animation
        setSmooth();
        
        bbox = _bbox;
        // transformations
        t_translate.set(0, 0);
        translate = t_translate;
        t_sc = sc = 1;

        scaleMin = _scaleMin;
        scaleMax = _scaleMax;
    };

    void setSmooth(float _damp = 0.09f)
    {
        damp = _damp;
    };

    void update()
    {
        if (bbox != ofRectangle())
        {
            t_translate.x = ofClamp(t_translate.x, bbox.x, bbox.x + bbox.width);
            t_translate.y = ofClamp(t_translate.y, bbox.y, bbox.y + bbox.height);
        }
        translate += (t_translate - translate) * damp;
        sc += (t_sc - sc) * damp;
    };
    
    ofRectangle getScreenToLocalView()
    {
        ofVec2f topLeft = screenToLocal(ofVec2f(0, 0));
        ofVec2f bottomp2 = screenToLocal(
                                  ofVec2f(ofGetWidth(), ofGetHeight()));
        return ofRectangle(topLeft.x, topLeft.y, bottomp2.x - topLeft.x,
                           bottomp2.y - topLeft.y);
    };

    ofVec2f localToScreen(ofVec2f p)
    {
        return p * sc + translate;
    };

    ofVec2f screenToLocal(ofVec2f p)
    {
        return (p - translate) / sc;
    };

    ofVec2f t_localToScreen(ofVec2f p)
    {
        return p * t_sc + t_translate;
    }
    ;

    ofVec2f t_screenToLocal(ofVec2f p)
    {
        return (p - t_translate) / t_sc;
    };

    void begin()
    {
        ofPushMatrix();

        ofTranslate(translate);
        ofScale(sc, sc);
    };

    void end()
    {
        ofPopMatrix();
    };

    void pan(ofVec2f direction)
    {
        t_translate += direction;
    };

    void zoom(ofVec2f p, float amount)
    {
        ofVec2f v = (t_translate - p);
        float pSc = t_sc;
        t_sc = ofClamp(t_sc * amount, scaleMin, scaleMax);
        t_translate = p + v * (t_sc / pSc);
    };

    float getScale()
    {
        return t_sc;
    };
    
    ofRectangle bbox;
    ofVec2f translate, t_translate;
    float sc, t_sc;
    float scaleMin, scaleMax;
    float damp;
};

#endif // TANGIBLEVIEW_H
