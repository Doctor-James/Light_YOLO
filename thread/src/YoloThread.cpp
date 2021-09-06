#include "YoloThread.h"
namespace zjl
{
    YoloThread::YoloThread(cameraThread *camera,serialPortReadWriteThread *serialPortReadWriteThread):
    serialPortReadWriteThread_(serialPortReadWriteThread),camera_(camera)
    {
        std::string engine_name = "/home/nvidia/code/Light/Yolo/New_Camera.engine";
 //      std::string engine_name = "/home/nvidia/code/Light/Yolo/3rd_garbage.engine";
       	bool engine_init_flag = engine_init(engine_name);
        transform_ = new transform(serialPortReadWriteThread_);
        start(10000);
    }

    void YoloThread::process()
    {
        srcImage = camera_->getFrame();
        srcImage_clone = srcImage.clone();
        bool yolo_status = false;
        //srcImage = cv::imread("/home/nvidia/TensorRT_yolov5_4.0/samples/t.jpg");
        if(srcImage_clone.empty())
        {
            return;
        
	    }
//	std::cout<<"dd"<<std::endl;
	//        cv::namedWindow("ss",0);
//        cv::imshow("ss",srcImage);
//        cv::waitKey(10);
//        std::cout<<"ss"<<std::endl;
        double yaw,pitch;
        int class_;

    receiveData_ = serialPortReadWriteThread_->getReceiveMsg();
	yolo_status = yolo_main(srcImage_clone,yaw,pitch,class_,(int)receiveData_.flag_number);
//  	cv::namedWindow("srcImage",0);
//  	cv::imshow("srcImage",srcImage_clone);
//  	cv::waitKey(10);
 

  	
	if(yolo_status)
        {
            transform_->Yolo_trans(pitch,yaw,class_);
        }
    }

}
