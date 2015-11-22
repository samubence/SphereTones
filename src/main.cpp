#include "ofMain.h"
#include "ofApp.h"

#ifdef TARGET_ANDROID
#include "ofGLProgrammableRenderer.h"

int main(){
    //ofSetCurrentRenderer(ofPtr<ofBaseRenderer>(new ofGLProgrammableRenderer()));
	//ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
    ofSetupOpenGL(1024, 768, OF_WINDOW);
    ofRunApp( new ofApp() );
    return 0;
}


#include <jni.h>

//========================================================================
extern "C"{
    void Java_cc_openframeworks_OFAndroid_init( JNIEnv*  env, jobject  thiz ){
        main();
    }
}
#else
int main()
{
    int resx = 1080 / 2;
    int resy = 1920 / 2;
    ofSetupOpenGL(resx, resy, OF_WINDOW);
    ofRunApp(new ofApp() );
}
#endif
