#ifndef __Sampler_h__
#define __Sampler_h__

#include "ofMain.h"
#include "ofxPd.h"

using namespace pd;

#define MAX_VOLUME 0.2f
#define MIN_SPEED 0.1f
#define MAX_SPEED 3.0f

class Sampler : public PdReceiver, public PdMidiReceiver
{
public:
    Sampler();
    static void setup(ofBaseApp * appPtr, string dirName);
    static int getNumOfSamples();
    static void play(int id, float volume = MAX_VOLUME, float speed = 1);
    static void audioReceived(float * input, int bufferSize, int nChannels);
    static void audioRequested(float * output, int bufferSize, int nChannels);
    static void updateFilters(float pct0, float pct1, float pct2);
    static void startRecording(string _fileName);
    static void stopRecording();
    static string getFileName();
    static bool isRecording();

private:
    static int numOfSamples;
    static ofxPd pd;
    static string fileName;
    static bool recording;
};

#endif
