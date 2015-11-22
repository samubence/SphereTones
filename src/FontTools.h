#ifndef __FontTools_h__
#define __FontTools_h__

#include "ofMain.h"

class FontTools
{
    public:

        static void loadFont(string fname, float size)
        {
            font.loadFont(fname, size, true, false, false);
        };

        static void drawText(string text)
        {
            float width = font.stringWidth(text);
            float height = font.stringHeight(text);

            font.drawString(text, -width / 2., height / 2.);
        };

        static ofTrueTypeFont font;
};

#endif

