#ifndef __SOLVE_TRANSFROM_H
#define __SOLVE_TRANSFROM_H

#include<opencv2/opencv.hpp>
#include <vector>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "sophus/se3.h"
#include "sophus/so3.h"
#include "pthread.h"
#include <algorithm>
#include "serialPortReadWriteThread.h"
namespace zjl
{

    class transform
    {
    public:
        transform(serialPortReadWriteThread *serialPortReadWriteThread_);
        Sophus::SE3 armor_;
        Sophus::SE3 last_armor_;
        void setArmor2Cam(Sophus::SE3 armor2cam);

        void setimu(float pitch, float yaw, float roll);
        void Yolo_trans(double pitch,double yaw,int class_);
        void tran_aim(cv::Point2f center_point,int send_color);
        void tran_error();
        double pitch;
        double yaw;
        double temp;
        receiveData receiveData_1;
        receiveData receiveData_2;
    private:
        Sophus::SE3 world_;
        Sophus::SE3 imu_;
        Sophus::SE3 camera_;
        Sophus::SE3 gimbal_;
        Sophus::SE3 shooter_;
        Sophus::SE3 gimbal_to_cam_;
        FILE *file_angle;
        FILE *file_pitch;
        FILE *file_yaw;
        std::ifstream *fangle;
        std::ifstream *fpitch;
        std::ifstream *fyaw;
        serialPortReadWriteThread *serialPortReadWriteThread_;


        // kalman filter
        // x y z theta_x theta_y theta_z
        cv::Mat measurement_;
        cv::KalmanFilter KF_;
        cv::Mat state_;
        cv::Mat processNoise_;
        cv::Mat KF_prediction=cv::Mat::zeros(4, 1, CV_32F);
        cv::Mat prediction=cv::Mat::zeros(2, 1, CV_32F);
        float delta_t;
        float distance;
        int debug_ = 0;

    };
}






#endif //__SOLVE_TRANSFROM_H
