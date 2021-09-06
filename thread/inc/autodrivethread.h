#ifndef INTELLIGENT_DRIVING_AUTODRIVE_H
#define INTELLIGENT_DRIVING_AUTODRIVE_H

#include <opencv2/opencv.hpp>
#include "thread.h"
#include "park.h"
#include "cameraThread.h"

using namespace std;
using namespace cv;


namespace zjl
{
    struct DNode
    {
    public:
        Color value;
        DNode *prev;
        DNode *next;
    public:
        DNode() { }
        DNode(Color value, DNode *prev, DNode *next) {
            this->value = value;
            this->prev  = prev;
            this->next  = next;
        }
    };

    typedef struct DNode *Nodeptr;
    class Autodrivethread:public thread
    {
    public:
        Autodrivethread(cameraThread *camera,serialPortReadWriteThread *serialPortReadWriteThread);
        void process() override;
        void setcolorpriority(Color color_);
    private:
        cameraThread *camera_ = nullptr;
        serialPortReadWriteThread *serialPortReadWriteThread_ = nullptr;
        Park *park;
        int status = 0;
        int count = 0;
	Mat srcImage;
        Mat src;
        Point3d point;
        receiveData receiveData_;
        double center_distance,delta;
        vector<Color> color_priority;
        DNode head_node;
        Nodeptr head_ptr;
        int sequence;
    };



}

#endif //INTELLIGENT_DRIVING_AUTODRIVE_H
