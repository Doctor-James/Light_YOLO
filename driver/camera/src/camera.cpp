#include "camera.h"
#include <iostream>
#include<opencv2/opencv.hpp>
#include <utility>

namespace zjl
{
    long long int camera::frame_id_ = 0;

    /**
     * @brief ���λ�ȡͼƬ
     * @return ͼƬ
     */
    cv::Mat picture::getFrame()
    {
        std::stringstream ss;
//        ss<<pic_path_<<"/"<<getFrameId()<<".jpg"; // ƴ���ַ���
//        ss<<pic_path_; // ƴ���ַ���
        auto pic = cv::imread(pic_path_);
        //updateId();
        return pic;
    }
    picture::picture(std::string pic_path)
    {
        pic_path_ = std::move(pic_path);
    }

    /**
     * @brief ��ȡ��Ƶ�е�һ֡
     * @return ͼƬ
     */
    cv::Mat video::getFrame()
    {
        cap_>>frame_;
        updateId();
        return frame_;
    }
    /**
     * @brief ���캯��
     * @param video_path ��Ƶ·��
     */
    video::video(const std::string& video_path)
    {
        cap_.open(video_path);
        if(!cap_.isOpened())//�����Ƶ�����������򷵻�
        {
            std::cerr<<"error !!! can not open the video!"<<std::endl;
            return ;
        }
        else
        {
            std::cout<<"open video success !"<<std::endl<<"path:"<<video_path<<std::endl;
        }
    }


    /**
 * @brief ��ȡ��Ƶ�е�һ֡
 * @return ͼƬ
 */
    cv::Mat Logitech::getFrame()
    {
        cap_>>frame_;
        updateId();
        return frame_;
    }
    /**
     * @brief ���캯��
     * @param video_path ��Ƶ·��
     */
    Logitech::Logitech(const std::string& camera_path)
    {
        cap_.open(camera_path);
        if(!cap_.isOpened())//�����Ƶ�����������򷵻�
        {
            std::cerr<<"error !!! can not open the camera!"<<std::endl; 
 	    cap_.open("/dev/video1");
	   // auto i = cap_.get(CV_CAP_PROP_EXPOSURE);
	   // std::cout << "expose_time:" <<(int) i <<std::endl;
//cap_.set(cv::CAP_PROP_AUTO_EXPOSURE, 0.25); // where 0.25 means "manual exposure, manual iris"
 
//cap_.set(CV_CAP_PROP_EXPOSURE, -1);
	    //return;
        }
        else
        {
    

//cap_.set(cv::CAP_PROP_AUTO_EXPOSURE, 0); // where 0.25 means "manual exposure, manual iris"

//cap_.set(CV_CAP_PROP_EXPOSURE, -8);

	     	std::cout<<"open camera success !"<<std::endl<<"path:"<<camera_path<<std::endl;
        }
    }

}





