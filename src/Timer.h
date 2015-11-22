#ifndef __Timer_h__
#define __Timer_h__

#include "ofMain.h"

class Timer
{
    public:

        void start(int _interval = 0)
        {
            started = ofGetElapsedTimeMillis();
            interval = _interval;
        };

        float getPercent()
        {
            if (interval == 0) return 0;
            return ofClamp((ofGetElapsedTimeMillis() - started) / (float)interval, 0, 1);
        };

        int getMillis()
        {
            return ofGetElapsedTimeMillis() - started;
        }

        int started, interval;
};

#endif

