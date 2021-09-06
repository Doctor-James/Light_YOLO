#ifndef YOLOTHREAD_H
#define YOLOTHREAD_H

#include "serialPort.h"
#include <mutex>
#include <thread>
#include "thread.h"
#include<opencv2/opencv.hpp>
#include"serialPortReadWriteThread.h"
#include "transform.h"
#include "Yolov5.h"
#include "cameraThread.h"
namespace zjl
{
    class YoloThread:public thread
    {
    public:
        YoloThread(cameraThread *camera,serialPortReadWriteThread *serialPortReadWriteThread);
        ~YoloThread();
        void process() override;
    private:
        transform *transform_;
        serialPortReadWriteThread *serialPortReadWriteThread_;
        cameraThread *camera_;
        cv::Mat srcImage;
        cv::Mat srcImage_clone;
        receiveData receiveData_;
    };
}
#endif //__SERIAL_PORT_THREAD_H
