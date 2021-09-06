#include "Light.h"

namespace zjl
{
    Light::Light()
    {
        serialPortReadWriteThread_ = new serialPortReadWriteThread();
        cameraThread_ = new cameraThread(LOGITECH);
        Autodrivethread_ = new Autodrivethread(cameraThread_,serialPortReadWriteThread_);
        YoloThread_ = new YoloThread(cameraThread_,serialPortReadWriteThread_);
    }
    Light::~Light()
    {
        delete cameraThread_;
        delete Autodrivethread_;
	    delete serialPortReadWriteThread_;
//	    delete YoloThread_;
    }
}
