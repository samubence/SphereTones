#include "Sampler.h"

int Sampler::numOfSamples = 9;
ofxPd Sampler::pd;
string Sampler::fileName;
bool Sampler::recording = false;

Sampler::Sampler()
{

}

void Sampler::setup(ofBaseApp * appPtr, string dirName)
{
#ifdef TARGET_ANDROID
    int sampleRate = 44100/2;
#else
    int sampleRate = 44100/1;
#endif
    int ticksPerBuffer = 8;	// 8 * 64 = buffer len of 512

    ofSoundStreamSetup(2, 0, appPtr, sampleRate, ofxPd::blockSize()*ticksPerBuffer, 3);
    
    pd.init(2, 1, sampleRate, ticksPerBuffer);
    pd.start();
    
    Patch patch = pd.openPatch("pd/main.pd");
    
    fileName = "temp.wav";
    recording = false;
}

int Sampler::getNumOfSamples()
{
    return numOfSamples;
}

void Sampler::updateFilters(float pct0, float pct1, float pct2)
{
    //Sampler::pd.sendFloat( "delayfx", pct0 );
    Sampler::pd.sendFloat( "rawfx", ofClamp(pct0 + pct1, 0, 1) );
    Sampler::pd.sendFloat( "reverbfx", pct2 );
}

void Sampler::startRecording(string _fileName)
{
	fileName = _fileName;
    List mList;
    mList.addSymbol(fileName);
    pd.sendList("startRec", mList);
    recording = true;
}

void Sampler::stopRecording()
{
	if (recording)
	{
		pd.sendBang("stopRec");
	}
	recording = false;
}

bool Sampler::isRecording()
{
	return recording;
}

string Sampler::getFileName()
{
    return fileName;
}

void Sampler::play(int id, float volume, float speed)
{
    if (numOfSamples == 0 || id < 0 || volume <= 0) return;
    id = id % numOfSamples;//(int)ofClamp(id, 0, numOfSamples);
    volume = ofMap(volume, 0, 1, 0, MAX_VOLUME, true);
    speed = ofClamp(speed, MIN_SPEED, MAX_SPEED);
    List mList;
    mList.addFloat(speed);
    mList.addFloat(volume);
    pd.sendList("pitchVol" + ofToString(id), mList);
    //ofLogNotice() << "play: " << id << " " << volume << " " << speed << " " << endl;
};

void Sampler::audioReceived(float * input, int bufferSize, int nChannels)
{
    pd.audioIn(input, bufferSize, nChannels);
}

void Sampler::audioRequested(float * output, int bufferSize, int nChannels)
{
    pd.audioOut(output, bufferSize, nChannels);
}
