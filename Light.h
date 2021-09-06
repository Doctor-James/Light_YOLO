#ifndef __LIGHT__
#define __LIGHT__


#include "camera.h"
#include "autodrivethread.h"
#include "serialPortReadWriteThread.h"
#include "cameraThread.h"
#include "YoloThread.h"

namespace zjl
{
    class Light
    {
    public:
        Light();
        ~Light();
    private:
        Autodrivethread *Autodrivethread_;
        serialPortReadWriteThread *serialPortReadWriteThread_;
        cameraThread *cameraThread_;
        YoloThread *YoloThread_;
    };
}



#endif //__LIGHT__
