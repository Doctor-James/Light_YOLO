#include "cameraThread.h"
#define HLS
//#define WhiteBalance
namespace zjl{

    /**
     * @brief 根据配置文件初始化不同的类
     * @param config 设备配置参数
     */
    cameraThread::cameraThread(int type_)
    {
        switch (type_)
        {
            case LOGITECH:
//                try
//                {
                    cam_ = new Logitech("/dev/video0");
                    if(cam_ == NULL)
                    {
                        cam_ = new Logitech("/dev/video1");
                    }
//                }
//                catch (...)
//                {
//                    std::cout<<"ccc"<<std::endl;
//                    cam_ = new Logitech("/dev/video1");
//                }
                start(10000);//1KHZ
                break;
            case PIC:
                cam_ = new picture("../img/3.jpg");
                start(10000);//100帧相机
                break;
            case VIDEO:
                cam_ = new video("/home/nvidia/code/Light/video/3.mp4");
                start(30000);//100帧相机
                break;
            default:
                break;
        }
    }

    /**
     * @brief 多线程获取相机信息
     */
    void cameraThread::process()
    {
//        counter_.countEnd();
//        std::cout<<1000.0f/counter_.getTimeMs()<<std::endl;
//        counter_.countBegin();
        cv::Mat temp=cam_->getFrame();
       if(temp.empty())
            return;
//        cv::imshow("ss",frame_);
//        cv::waitKey(20);
//
//
//

#ifdef HLS
       std::vector<cv::Mat> imageHLS;
cv::Mat src,imageHLS_,src_;
	cv::cvtColor(temp,src,CV_BGR2HLS);
        //RGB三通道分离
	cv::split(src, imageHLS);

        //调整RGB三个通道各自的值
        imageHLS[0] = imageHLS[0] * (float)50/50.f;
        imageHLS[1] = imageHLS[1] * 1.2;
        imageHLS[2] = imageHLS[2] * 0.8;
        //RGB三通道图像合并
	cv::merge(imageHLS, imageHLS_);
	cv::cvtColor(imageHLS_,temp,CV_HLS2BGR);
#endif
#ifdef WhiteBalance
       std::vector<cv::Mat> imageHLS;
	//RGB三通道分离
       cv::split(temp, imageRGB);
 
	//求原始图像的RGB分量的均值
	double R, G, B;
	B = mean(imageRGB[0])[0];
	G = mean(imageRGB[1])[0];
	R = mean(imageRGB[2])[0];
 
	//需要调整的RGB分量的增益
	double KR, KG, KB;
	KB = (R + G + B) / (3 * B);
	KG = (R + G + B) / (3 * G);
	KR = (R + G + B) / (3 * R);
 
	//调整RGB三个通道各自的值
	imageRGB[0] = imageRGB[0] * KB;
	imageRGB[1] = imageRGB[1] * KG;
	imageRGB[2] = imageRGB[2] * KR;
 
	//RGB三通道图像合并
	cv::merge(imageRGB, temp); 
    
#endif
       frame_ = temp.clone();

    //  cv::namedWindow("srcIma",0);
    //  cv::imshow("srcIma",temp);
    //  cv::waitKey(10);

	//	std::cout<<"ss"<<std::endl;
//        mutex_pic_.lock();
//        frame_ = temp.clone();
//        mutex_pic_.unlock();
//        mutex_pic2_.lock();
//        frame2_ = frame_.clone();
//        mutex_pic2_.unlock();

    }
    /**
     * @brief 返回图片信息，相机和非相机两种情况
     * @return 图片信息
     */
    cv::Mat cameraThread::getFrame()
    {
        cv::Mat temp1;
//        mutex_pic_.lock();
        temp1 = frame_;
//        mutex_pic_.unlock();

        return temp1;
    }

}
