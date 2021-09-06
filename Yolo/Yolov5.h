#ifndef YOLOV5_H_
#define YOLOV5_H_

#include <iostream>
#include <chrono>
#include "cuda_utils.h"
#include "logging.h"
#include "utils.h"
#include "calibrator.h"
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "NvInfer.h"
#include "yololayer.h"
#include "NvInferRuntime.h"
bool yolo_main(cv::Mat &src,double &yaw,double &pitch,int &class_,int double_flag);
bool engine_init(std::string engine_name);
typedef struct single_img_result{
    int class_;
    cv::Rect box_;
    cv::Point Point_;
}result;
struct targetPose
{
    double angle;
    double distance;
    result result_;
    bool status = false;
};
#endif

