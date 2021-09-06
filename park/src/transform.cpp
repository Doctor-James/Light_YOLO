
#include "eigen3/Eigen/Dense"
#include <opencv2/core/eigen.hpp>
#include "transform.h"

#define BIAS_PITCH 0
#define BIAS_YAW 0
#define KF_REBOOT_THRESH 0.5
#define KF_STATE_X KF_.statePost.at<float>(0)
#define KF_STATE_Y KF_.statePost.at<float>(1)
#define KF_STATE_VX KF_.statePost.at<float>(2)
#define KF_STATE_VY KF_.statePost.at<float>(3)

cv::Point2d measurePoint;
cv::Point2d statePoint;

cv::Point2f pointBuff[6];
float zBuff[9] = {0};
float filterCOE[6]={0.0264077249,0.1405313627,0.3330609123,0.3330609123,0.1405313627,0.0264077249};
float zfilterCOE[9]={0.01720767,0.0475656475,0.1221292039,0.19813310412,0.22992874818,0.19813310412,0.1221292039,0.0475656475,0.01720767};
namespace zjl {
    extern const double table_angle[2937];
    extern const double table_pitch[2937];
    extern const double table_yaw[2937];
    sendData sendData_;
    sendData sendData_2;
    transform::transform(serialPortReadWriteThread *serialPortReadWriteThread_):
    serialPortReadWriteThread_(serialPortReadWriteThread_)
    {
        world_ = Sophus::SE3(Eigen::Matrix3d::Identity(), Eigen::Vector3d(0, 0, 0));
        imu_ = Sophus::SE3(Eigen::Matrix3d::Identity(), Eigen::Vector3d(0, 0, 0));
        //辅瞄暂时不使用IMU
        // 如果要用陀螺仪 gimbal_ = imu_;
        gimbal_ = imu_;
        //@TODO 改成用配置文件
        //Eigen::Vector3d shooter_trans(0,0.1,0.05);
        Eigen::Vector3d camera_trans(0, 0.1085, 0.05941);

        //shooter_ = Sophus::SE3(Eigen::Matrix3d::Identity(),shooter_trans);
        gimbal_to_cam_ = Sophus::SE3(Eigen::Matrix3d::Identity(), camera_trans);

        armor_ = world_;
        measurement_ = cv::Mat::zeros(4, 1, CV_32F);
        KF_ = cv::KalmanFilter(4, 4, 0, CV_32F);
        state_ = cv::Mat::zeros(4, 1, CV_32F);
        processNoise_ = cv::Mat::zeros(4, 1, CV_32F);

        KF_.transitionMatrix = (cv::Mat_<float>(4, 4) <<
                                                      1, 0, 1, 0,//A 状态转移矩阵
                0, 1, 0, 1,
                0, 0, 1, 0,
                0, 0, 0, 1);
        setIdentity(KF_.measurementMatrix);
        setIdentity(KF_.processNoiseCov, cv::Scalar::all(1e-5));
        setIdentity(KF_.measurementNoiseCov, cv::Scalar::all(1e-1));
        setIdentity(KF_.errorCovPost, cv::Scalar::all(1));
        randn(KF_.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));
        sendData_.flag_1 = 0;
        sendData_.flag_2 = 0;

    }

    void transform::setimu(float pitch, float yaw, float roll) {
        Eigen::Matrix3d rotation_matrix3;
        rotation_matrix3 = Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY()) *
                           Eigen::AngleAxisd(yaw, Eigen::Vector3d::UnitZ()) *
                           Eigen::AngleAxisd(pitch, Eigen::Vector3d::UnitX());
        gimbal_ = Sophus::SE3(rotation_matrix3, Eigen::Vector3d(0, 0, 0));
    }

    void transform::setArmor2Cam(Sophus::SE3 armor2cam) {
/*        counter_.countEnd();
std::cout<<counter_.getTimeMs()<<std::endl;
counter_.countBegin();

*/
        receiveData_1 = serialPortReadWriteThread_->getReceiveMsg();
        //std::cout<<"11pitch:"<<receiveData_.pitch/100.0f<<std::endl;
        float ppp = receiveData_1.pitch / 100.0f * 3.14159f / 180.0f;
        float yyy = receiveData_1.yaw / 100.0f * 3.14159f / 180.0f;
        setimu(ppp, yyy, 0);
//	std::cout<<"STM32 Pitch:"<<receiveData_.pitch<<std::endl;
//std::cout<<"STM32 Yaw:"<<receiveData_.yaw<<std::endl;
        camera_ = gimbal_ * gimbal_to_cam_;
        armor_ = camera_ * armor2cam;
        //armor_ = armor2cam;
//        std::cout<<"armor2cam"<<armor_.translation()<<std::endl;


        KF_.statePre.at<float>(0) = armor_.translation()[0];
        KF_.statePre.at<float>(1) = armor_.translation()[1];
        KF_.statePre.at<float>(2) = 0;
        KF_.statePre.at<float>(3) = 0;
        KF_.statePost.at<float>(0) = armor_.translation()[0];
        KF_.statePost.at<float>(1) = armor_.translation()[1];
        KF_.statePost.at<float>(2) = 0;
        KF_.statePost.at<float>(3) = 0;
        setIdentity(KF_.errorCovPost, cv::Scalar::all(1));
        setIdentity(KF_.errorCovPre, cv::Scalar::all(1));


        last_armor_.translation()[0] = armor_.translation()[0];
        last_armor_.translation()[1] = armor_.translation()[1];
        last_armor_.translation()[2] = armor_.translation()[2];

        KF_prediction = KF_.predict();

/**************ADD FIR************************/
        for (int i = 0; i < 5; i++) {
            pointBuff[i] = pointBuff[i + 1];
        }
        for (int i = 0; i < 8; i++) {
            zBuff[i] = zBuff[i + 1];
        }
        pointBuff[5] = cv::Point2f(armor_.translation()[0], armor_.translation()[1]);
        zBuff[8] = armor_.translation()[2];
//printf("znew   %f3\n",armor_.translation()[2]);
        measurement_.at<float>(0) = 0;
        measurement_.at<float>(1) = 0;
        float z = 0;

        for (int i = 0; i < 6; i++) {
            measurement_.at<float>(0) += filterCOE[i] * (pointBuff[i].x);
            measurement_.at<float>(1) += filterCOE[i] * (pointBuff[i].y);
        }
        for (int i = 0; i < 9; i++) {
            z += zfilterCOE[i] * (zBuff[i]);

        }

/**********************************************/


        measurePoint.x = measurement_.at<float>(0) * 100 + 512;
        measurePoint.y = -measurement_.at<float>(1) * 100 + 384;
        //printf("measX	%f3	measY	%f3\n",armor_.translation()[0],	armor_.translation()[1]);
        //printf("filterX	%f3	filterY	%f3\n",measurePoint.x,measurePoint.y);
        //printf("%f3	%f3\n",measurePoint.x,measurePoint.y);
        KF_.correct(measurement_);

//        float distance = sqrt(pow(KF_prediction.at<float>(0),2) + pow(KF_prediction.at<float>(1),2) + pow(KF_prediction.at<float>(2),2));
        float distance = sqrt(pow(armor_.translation()[0], 2) + pow(armor_.translation()[1], 2) + pow(armor_.translation()[2], 2));
        float predictX = KF_STATE_X;//+ KF_STATE_VX*580*delta_t;
        float predictY = KF_STATE_Y;//+ KF_STATE_VY*580*delta_t;

        distance = sqrt(pow(predictX, 2) + pow(predictY, 2));// + pow(armor_.translation()[2],2));

//       std::cout<<delta_t<<std::endl;

        statePoint = cv::Point2d(KF_.statePost.at<float>(0) * 100 + 512, -KF_.statePost.at<float>(1) * 100 + 384);


//
//        std::cout<<"x:"<<KF_prediction.at<float>(2)<<std::endl;
//        std::cout<<"y"<<KF_prediction.at<float>(3)<<std::endl;
        // 在这里加发送串口信息，以及结算角度
//      pitch = atan2(armor_.translation()[2], distance)*180.0f/3.14159f+1.5;
        pitch = atan2(armor_.translation()[2], distance) * 180.0f / 3.14159f + BIAS_PITCH;
//        pitch = atan2(armor_.translation()[2], armor_.translation()[1])*180.0f/3.14159f;

//        yaw = -atan2(armor_.translation()[0], armor_.translation()[1])*180.0f/3.14159f;
        yaw = -atan2(predictX, predictY) * 180.0f / 3.14159f + BIAS_YAW;


            sendData_.x = predictX * 100;
            sendData_.y = predictY * 100;
            sendData_.flag_1 ++;
            if(sendData_.flag_1>50)
            {
                sendData_.flag_1 =0;
            }

            {
/*	std::cout<<"send.y:"<<armor_.translation()[1]<<"send.pitch:"<<sendData_.pitch<<"send.distance:"<<sendData_.distance <<"send.z:"<<armor_.translation()[2]<<"send.x:"<<armor_.translation()[0]<<std::endl;
*/


/*
		printf("STM Pitch	%f3	STM Yaw	%f3\n",ppp*180.0f/3.14159f,yyy*180.0f/3.14159f);
		printf("send Pitch	%f3	sendYaw	%f3\n",pitch,yaw);
		printf("predictX	%f3	predictY	%f3\n",predictX,predictY);
		printf("distance	%f3	z	%f3\n",distance,z);
*/
//std::cout<<gimbalCam[0]<<std::endl;
//std::cout<<gimbalCam[1]<<std::endl;
//std::cout<<gimbalCam[2]<<std::endl;
                serialPortReadWriteThread_->setSendMsg(sendData_);
            }

    }


    void transform::Yolo_trans(double pitch,double yaw,int class_)
    {
        receiveData_2 = serialPortReadWriteThread_->getReceiveMsg();
         sendData_2.flag_1 ++;
	        if(sendData_2.flag_1>50)
		               {
			                   sendData_2.flag_1 =0;
			            }

	sendData_2.pitch = receiveData_2.pitch + pitch;
        sendData_2.yaw = receiveData_2.yaw + yaw;
        sendData_2.class_ = class_;
        serialPortReadWriteThread_->setSendMsg_2(sendData_2);
    }

    void transform::tran_aim(cv::Point2f center_point,int send_color)
    {

        sendData_.x = center_point.x ;
        sendData_.y = 0;//center_point.y ;
	sendData_.send_color = send_color;
        sendData_.flag_1 ++;
        if(sendData_.flag_1>50)
        {
            sendData_.flag_1 =0;
        }
        serialPortReadWriteThread_->setSendMsg(sendData_);
    }

    void transform::tran_error()
    {
        sendData_.x =0;
        sendData_.y = 10;//center_point.y ;
        sendData_.flag_1 ++;
        if(sendData_.flag_1>50)
        {
            sendData_.flag_1 =0;
        }
        serialPortReadWriteThread_->setSendMsg(sendData_);

    }
}
