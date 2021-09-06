#ifndef __CAMERA_THREAD_H
#define __CAMERA_THREAD_H

#include "camera.h"
#include <mutex>
#include <thread>
#include "thread.h"

namespace zjl{

class cameraThread:public camera,public thread
{
public:
    cameraThread(int type_);
    cv::Mat getFrame() override;
    int getType() override{ return type_;}
    inline double getUpdateTime() override{ return cam_->getUpdateTime();}
    inline int getFrameId() override{ return cam_->getFrameId();}
private:
    int type_;
    camera * cam_{};
    void process();
};
}


#endif //__CAMERA_THREAD_H
