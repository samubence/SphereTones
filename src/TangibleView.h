#ifndef TANGIBLEVIEW_H
#define TANGIBLEVIEW_H

class TangibleTransformPoints
{
public:
    void setup(ofVec2f _p1, ofVec2f _p2)
    {
        p1 = _p1;
        p2 = _p2;
    };
    
    ofVec2f p1, p2;
};

class TangibleView
{
public:

    void setup(float _scaleMin = 0.1, float _scaleMax = 5, ofRectangle _bbox = ofRectangle(), ofRectangle _screenView = ofGetCurrentViewport())
    {
        // animation
        setSmooth();
        bbox = _bbox;
        screenView = _screenView;
        hasBorder = bbox != ofRectangle();
        if (!hasBorder) bbox.set(0, 0, 1000, 1000);
        
        bboxDiagonal = sqrtf(bbox.width * bbox.width + bbox.height * bbox.height);
        
        transformPoints.setup(ofVec2f(0, 0), ofVec2f(transformPoints.p1.x + bbox.width, transformPoints.p1.y + bbox.height));

        // transformations
        t_translate.set(0, 0);
        translate = t_translate;
        t_sc = sc = 1;
        t_r = r = 0;

        scaleMin = _scaleMin;
        scaleMax = _scaleMax;
    };

    void setSmooth(float _damp = 0.07f)
    {
        damp = _damp;
    };

    void update()
    {
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
    
    void applyTransformPoints(TangibleTransformPoints tp)
    {
        transformPoints = tp;
        calcTranstormation();
    };
    
    TangibleTransformPoints getTransformPoints()
    {
        return transformPoints;
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

        transformPoints.p1 += direction;
        transformPoints.p2 += direction;

        calcTranstormation();
    };

    void zoom(ofVec2f p, float amount)
    {
        if (t_sc <= scaleMin && amount < 1)
        {
            t_sc = scaleMin;
            return;
        }
        if (t_sc >= scaleMax && amount > 1)
        {
            t_sc = scaleMax;
            return;
        }
        if (amount > 1)
        {
            transformPoints.p1 += (transformPoints.p1 - p) * (amount - 1);
            transformPoints.p2 += (transformPoints.p2 - p) * (amount - 1);
        }
        else
        {
            transformPoints.p1 += (transformPoints.p1 - p) * -(1 - amount);
            transformPoints.p2 += (transformPoints.p2 - p) * - (1 - amount);
        }

        calcTranstormation();
    };

    float getScale()
    {
        return t_sc;
    };

    void calcTranstormation()
    {
        t_translate = transformPoints.p1;
        t_sc = ofClamp(ofDist(transformPoints.p1.x, transformPoints.p1.y, transformPoints.p2.x, transformPoints.p2.y) / bboxDiagonal, scaleMin, scaleMax);
        /*
        if (hasBorder)
        {
            ofVec2f topLeftScreen = t_localToScreen(ofVec2f(bbox.x, bbox.y));
            ofVec2f rightBottomScreen = t_localToScreen(
                                            ofVec2f(bbox.x + bbox.width, bbox.y + bbox.height));
            
            bool outLeft = false, outRight = false, outTop = false, outBottom = false;
            
            if (topLeftScreen.x > screenView.x)
            {
                float v = screenView.x - topLeftScreen.x;
                transformPoints.p1.x += v;
                transformPoints.p2.x += v;
                outLeft = true;
            }
            if (topLeftScreen.y > screenView.y)
            {
                float v = screenView.y - topLeftScreen.y;
                transformPoints.p1.y += v;
                transformPoints.p2.y += v;
                outTop = true;
            }
            
            if (rightBottomScreen.x < screenView.x + screenView.getWidth())
            {

                float v = screenView.x + screenView.getWidth() - rightBottomScreen.x;
                transformPoints.p1.x += v;
                transformPoints.p2.x += v;
                outRight = true;
            }
            
            if (rightBottomScreen.y < screenView.y + screenView.getHeight())
            {

                float v = screenView.y + screenView.getHeight() - rightBottomScreen.y;
                transformPoints.p1.y += v;
                transformPoints.p2.y += v;
                outBottom = true;
            }

            if (outLeft && outRight)
            {
                float d = transformPoints.p2.x - transformPoints.p1.x;
                float cx = screenView.x + screenView.width / 2.f;
                transformPoints.p1.x = cx - d / 2.;
                transformPoints.p2.x = transformPoints.p1.x + d;
            }
            
            if (outTop && outBottom)
            {
                float d = transformPoints.p2.y - transformPoints.p1.y;
                float cy = screenView.y + screenView.height / 2.f;
                transformPoints.p1.y = cy - d / 2.;
                transformPoints.p2.y = transformPoints.p1.y + d;
            }
         
            t_translate = transformPoints.p1;
            //t_sc = ofDist(transformPoints.p1.x, transformPoints.p1.y, transformPoints.p2.x, transformPoints.p2.y) / bboxDiagonal;
            t_sc = ofClamp(ofDist(transformPoints.p1.x, transformPoints.p1.y, transformPoints.p2.x, transformPoints.p2.y) / bboxDiagonal, scaleMin, scaleMax);
        }
         */
    };

    TangibleTransformPoints transformPoints;

    ofVec2f translate, t_translate;
    float sc, r, t_sc, t_r;
    float scaleMin, scaleMax;
    float damp;
    ofRectangle bbox;
    bool hasBorder;
    ofRectangle screenView;
    float bboxDiagonal;

};

#endif // TANGIBLEVIEW_H
