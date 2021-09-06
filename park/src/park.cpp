#include "park.h"
using namespace cv;
using namespace std;
#define HSV
//#define PLAN_A //通过角度解算位置，暂时弃用
#define PLAN_B //通过停车位背板上边缘对齐
#define HSV_DEBUG
#define MAX_PARA 3
#define MAX_LENTH 20
//#define RGB
namespace zjl
{
    Park::Park(serialPortReadWriteThread *serialPortReadWriteThread):
            serialPortReadWriteThread_(serialPortReadWriteThread)
    {
        transform_ = new transform(serialPortReadWriteThread_);
    }
    bool Park::initial()
    {
        FileStorage fs("../para/camera.xml", FileStorage::READ);
        if (!fs.isOpened())
        {
            cout << "can't open xml file" << endl;
            return false;
        }
        fs["cameraMatrix"] >> cameraMatrix;
        fs["distCoeffs"] >> distCoeffs;

	fs.release();
        return true;
    }

    void Park::set_color(Color color,int flag_sun)
    {
        color_ = color;

    if(flag_sun == 1) // no_sun
    {
      if (color == blue)
        {
		iLowH = 89, iLowS = 190, iLowV = 0, iHighH = 154, iHighS = 255, iHighV = 255;

  //   iLowH = 96, iLowS = 50, iLowV =0, iHighH = 154, iHighS = 255, iHighV = 255;
     	}
        if (color == red)
        {
            iLowH = 0, iLowS = 0, iLowV = 0, iHighH = 30, iHighS = 255, iHighV = 255;
//            iLowH = 0, iLowS = 140, iLowV = 70, iHighH = 20, iHighS = 255, iHighV = 255;

	}
        if (color == green)
        {
            iLowH = 45, iLowS =80 , iLowV = 0, iHighH = 80, iHighS = 255, iHighV = 255;
      // iLowH = 40, iLowS =40 , iLowV = 0, iHighH = 115, iHighS = 255, iHighV = 210;

       	}
        if (color == gray)
        {
//    iLowH = 76, iLowS = 80, iLowV = 0, iHighH = 180, iHighS = 197, iHighV = 255;

    iLowH = 76, iLowS = 100, iLowV = 0, iHighH = 180, iHighS = 197, iHighV = 255;

      	}
    }

    else if(flag_sun == 2) // sun
    {
              if (color == blue)
        {

     iLowH = 96, iLowS = 50, iLowV =0, iHighH = 154, iHighS = 255, iHighV = 255;
     	}
        if (color == red)
        {
            iLowH = 0, iLowS = 0, iLowV = 0, iHighH = 30, iHighS = 255, iHighV = 255;

	}
        if (color == green)
        {
            iLowH = 45, iLowS =80 , iLowV = 0, iHighH = 80, iHighS = 255, iHighV = 255;

       	}
        if (color == gray)
        {
    iLowH = 76, iLowS = 80, iLowV = 0, iHighH = 180, iHighS = 197, iHighV = 255;


      	}
    }
  

    }

    void Park::get_newcandidate_rgb(cv::Mat &src)
    {
        srcImage =src.clone() ;
        vector<Mat> rgb_vec;
        Mat subtract_dst;
        split(srcImage, rgb_vec);
        if(color_ == blue)
        {
            cv::subtract(rgb_vec[0], rgb_vec[2], subtract_dst);
            cv::threshold(subtract_dst, imgThresholded, 80, 255, cv::THRESH_BINARY );      //thresh_在config里面配置
        }
        else if(color_ == red)
        {
            cv::subtract(rgb_vec[2], rgb_vec[0], subtract_dst);
            cv::threshold(subtract_dst, imgThresholded, 80, 255, cv::THRESH_BINARY );      //thresh_在config里面配置
        }

        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
        //dilate(imgThresholded,imgThresholded,element);
        morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);  //开操作
        vector<Point2f> corners;
        double qualityLevel = 0.01;
        double minDistance = 10;
        int blocksize = 3;
        int maxcornernumber = 300;
        double k = 0.04;
        goodFeaturesToTrack(imgThresholded,corners,maxcornernumber,qualityLevel,minDistance,Mat());
        for(int i=0;i<corners.size();i++)
        {
            circle(imgThresholded,corners[i],5,Scalar(255,0,0));
        }
//        imshow("src",imgThresholded);
//        waitKey(10);
    }

    void Park::get_newcandidate(cv::Mat &src)
    {
        srcImage =src.clone() ;
        imgThresholded = srcImage.clone();
        Mat dst= src.clone();
#ifdef HSV


        cvtColor(srcImage, imgHSV, COLOR_BGR2HSV); //BGR转换为HSV空间
        vector<Mat> hsvSplit;
        split(imgHSV, hsvSplit);
        equalizeHist(hsvSplit[2], hsvSplit[2]);
        merge(hsvSplit, imgHSV);
        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

#endif
#ifdef RGB
        vector<Mat> rgb_vec;
        Mat subtract_dst;
        split(srcImage, rgb_vec);
        if(color_ == blue)
        {
            cv::subtract(rgb_vec[0], rgb_vec[2], subtract_dst);
            cv::threshold(subtract_dst, imgThresholded, 80, 255, cv::THRESH_BINARY );      //thresh_在config里面配置
        }
        else if(color_ == red)
        {
            cv::subtract(rgb_vec[2], rgb_vec[0], subtract_dst);
            cv::threshold(subtract_dst, imgThresholded, 80, 255, cv::THRESH_BINARY );      //thresh_在config里面配置
        }
        else if(color_ == green)
        {
            cv::subtract(rgb_vec[1], rgb_vec[0], subtract_dst);
            cv::threshold(subtract_dst, imgThresholded, 40, 255, cv::THRESH_BINARY );      //thresh_在config里面配置
        }

#endif

        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
//            dilate(imgThresholded,imgThresholded,element);
        morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);  //开操作
//            morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element); //闭操作

        //清空上次保留的数据
        _contours.clear();    //保存四个点
        _squar_index.clear(); //保存计数
//            cv::Canny(imgThresholded,CannyImage,50,200,3);
//            imshow("video",CannyImage);
        vector<Vec4i> hierarchy;


#ifdef HSV_DEBUG
        namedWindow("imgThresholded",0);
        cv::createTrackbar( "iLowH : ","imgThresholded", &iLowH, 255, NULL);
        cv::createTrackbar( "iLowS : ","imgThresholded", &iLowS, 255, NULL);
        cv::createTrackbar( "iLowV : ","imgThresholded", &iLowV, 255, NULL);
        cv::createTrackbar( "iHighH : ","imgThresholded", &iHighH, 255, NULL);
        cv::createTrackbar( "iHighS : ","imgThresholded", &iHighS, 255, NULL);
        cv::createTrackbar( "iHighV : ","imgThresholded", &iHighV, 255, NULL);
        imshow("imgThresholded",imgThresholded);
	cv::waitKey(10);
#endif


        findContours(imgThresholded, _contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE); //求轮廓　

        //初步筛选符合要求的矩形
        for (int i = 0; i < _contours.size(); i++)
        {
            RotatedRect minrect = minAreaRect(_contours[i]); //最小外接矩形
            Point2f center;
            float radius = 0;
            minEnclosingCircle(_contours[i], center, radius); //求外接圆
            double circle_area = CV_PI * radius * radius;
            double rect_area = minrect.size.area();
            //排除点在边界的情况
            if (center.x < 10 || center.y < 10 || (srcImage.rows - center.y) < 10 || (srcImage.cols - center.x) < 10)
            {
                //cout << "第 " << i << "个不满足要求!" << endl;
                continue;
            }
//            _squar_index.push_back(i);
            if (rect_area >= 2000 && rect_area < 300000)
            {
                double ratio = rect_area / circle_area;
	//	std::cout<<ratio<<std::endl;
	       	if (ratio >= 0.4 && ratio <= 2)
                {
                    _squar_index.push_back(i);
                }
            }
        }

    }

    vector<Point2f> Park::pick_point_aim()
    {
        vector<Point2f> point_img;
        vector<vector<Point2f>> temp_img;
        for (size_t c = 0; c < _squar_index.size(); c++)
        {
            vector<Point> hull;
            convexHull(_contours[_squar_index[c]], hull); //凸包

            //pick four point
            vector<Point> squar;
            size_t num = hull.size();
            if (num < 4) //顶点数小于３
                continue;
            else if (num >= 4)
            {
                float max_area;
                for (int m = 0; m < num - 3; m++)
                {
                    for (int n = m + 1; n < num - 2; n++)
                    {
                        for (int j = n + 1; j < num - 1; j++)
                        {
                            for (int k = j + 1; k < num; k++)
                            {
                                vector<Point> squar_tmp;
                                squar_tmp.push_back(hull[m]);
                                squar_tmp.push_back(hull[n]);
                                squar_tmp.push_back(hull[j]);
                                squar_tmp.push_back(hull[k]);
                                if (m == 0 && n == 1 && j == 2 && k == 3)
                                {
                                    max_area = fabs(contourArea(Mat(squar_tmp)));
                                    squar.clear();
                                    squar = squar_tmp;
                                }
                                else
                                {
                                    float area = fabs(contourArea(Mat(squar_tmp)));
                                    if (area > max_area)
                                    {
                                        max_area = area;
                                        squar.clear();
                                        squar = squar_tmp;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (squar.size() != 4)
            { //如果顶点数不等于４
                continue;
            }
	   // std::cout<<"a"<<std::endl;
            int num_board = 0;
            for (int i = 0; i < squar.size(); i++)
            {
                num_board += (squar[i].x < 10) || (squar[i].x > srcImage.cols - 10) ||
                             (squar[i].y < 10) || (squar[i].y > srcImage.rows - 10);
            }
//            if (num_board > 0)
//            { //点在边界
//                continue;
//            }


            //给四点排序
            //vector<Point> squar_sort = point_Sort(squar);

//            if(!judgeparallelism(squar_sort))
//            {
//                continue;
//            }
            //sort_point(squar,squar_sort,srcImage);

            for (int i = 0; i < squar.size(); i++)
            {
                point_img.clear();
                for (size_t num_p = 0; num_p < squar.size(); num_p++)
                {
                    //                point_img.push_back(squar_sort[num_p] * (1 / minifactor));
                    point_img.push_back(squar[num_p]);
                }
            }
            temp_img.push_back(point_img);
        }
        if (temp_img.size() == 0)
        {
            return vector<Point2f>();
        }
/*
        if (temp_img.size() > 2)
        {
            point_img.clear();
            //        cout<<"识别的框太多！"<<endl;
            return vector<Point2f>();
        }

        if (temp_img.size() == 2)
        { //如果轮廓数等于2
            //double a1 = contourArea(temp_img[0], true);
            //double a2 = contourArea(temp_img[1], true);
            double a1 = temp_img[0].at(0).x;
            double a2 = temp_img[1].at(0).x;
            //        cout << "a1= " << a1 << endl << "a2= " << a2 << endl;
            if (a1 < a2)
            {
                point_img.clear();
                point_img = temp_img[0];
            }
            else
            {
                point_img.clear();
                point_img = temp_img[1];
            }
        }
        return point_img;
  */
	     if (temp_img.size() > 2)
        {
            int max_number=0;
            double max=0;
            for(int i=0;i<temp_img.size();i++)
            {
                if(max<contourArea(temp_img[i], true))
                {
                    max = contourArea(temp_img[i], true);
                    max_number = i;
                }
            }
            //point_img.clear();
            //        cout<<"识别的框太多！"<<endl;
            return temp_img[max_number];
        }

        if (temp_img.size() == 2)
        { //如果轮廓数等于2
            double b1 = contourArea(temp_img[0], true);
            double b2 = contourArea(temp_img[1], true);
	
	    if(b1>b2)
                {
                    point_img.clear();
                    point_img = temp_img[0];
                }
                else
                {
                    point_img.clear();
                    point_img = temp_img[1];
                }
                return point_img;
/*	 
            double a1 = temp_img[0].at(0).x;
            double a2 = temp_img[1].at(0).x;
            //        cout << "a1= " << a1 << endl << "a2= " << a2 << endl;
           if(number_choose == 2)
	   {
	    if (a1 > a2)
            {
                point_img.clear();
                point_img = temp_img[0];
            }
          else
            {
                point_img.clear();
                point_img = temp_img[1];
            }
	   }
	   else if(number_choose == 1)
	   {
	               if (a1 < a2)
            {
                point_img.clear();
                point_img = temp_img[0];
            }
          else
            {
                point_img.clear();
                point_img = temp_img[1];
            }

	   }
        
        return point_img;
  */ 

    }
	return point_img;
      	}


    vector<Point2f> Park::pick_point()
    {
        vector<Point2f> point_img;
        vector<vector<Point2f>> temp_img;
  
  	for (size_t c = 0; c < _squar_index.size(); c++)
        {
            vector<Point> hull;
            convexHull(_contours[_squar_index[c]], hull); //凸包

            //pick four point
            vector<Point> squar;
            size_t num = hull.size();
            if (num < 4) //顶点数小于３
                continue;
            else if (num >= 4)
            {
                float max_area;
                for (int m = 0; m < num - 3; m++)
                {
                    for (int n = m + 1; n < num - 2; n++)
                    {
                        for (int j = n + 1; j < num - 1; j++)
                        {
                            for (int k = j + 1; k < num; k++)
                            {
                                vector<Point> squar_tmp;
                                squar_tmp.push_back(hull[m]);
                                squar_tmp.push_back(hull[n]);
                                squar_tmp.push_back(hull[j]);
                                squar_tmp.push_back(hull[k]);
                                if (m == 0 && n == 1 && j == 2 && k == 3)
                                {
                                    max_area = fabs(contourArea(Mat(squar_tmp)));
                                    squar.clear();
                                    squar = squar_tmp;
                                }
                                else
                                {
                                    float area = fabs(contourArea(Mat(squar_tmp)));
                                    if (area > max_area)
                                    {
                                        max_area = area;
                                        squar.clear();
                                        squar = squar_tmp;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (squar.size() != 4)
            { //如果顶点数不等于４
                continue;
            }

            int num_board = 0;
            for (int i = 0; i < squar.size(); i++)
            {
                num_board += (squar[i].x < 10) || (squar[i].x > srcImage.cols - 10) ||
                             (squar[i].y < 10) || (squar[i].y > srcImage.rows - 10);
            }
            if (num_board > 0)
            { //点在边界
                continue;
            }


            //给四点排序
            //vector<Point> squar_sort = point_Sort(squar);

            //sort_point(squar,squar_sort,srcImage);

            for (int i = 0; i < squar.size(); i++)
            {
                point_img.clear();
                for (size_t num_p = 0; num_p < squar.size(); num_p++)
                {
                    //                point_img.push_back(squar_sort[num_p] * (1 / minifactor));
                    point_img.push_back(squar[num_p]);
                }
            }
            temp_img.push_back(point_img);
        }
        if (temp_img.size() == 0)
        {
            return vector<Point2f>();
        }

        if (temp_img.size() > 2)
        {
            point_img.clear();
            //        cout<<"识别的框太多！"<<endl;
            return vector<Point2f>();
        }

        if (temp_img.size() == 2)
        { //如果轮廓数等于2
            double a1 = contourArea(temp_img[0], true);
            double a2 = contourArea(temp_img[1], true);
            //        cout << "a1= " << a1 << endl << "a2= " << a2 << endl;
            if (a1 > a2)
            {
                point_img.clear();
                point_img = temp_img[0];
            }
            else
            {
                point_img.clear();
                point_img = temp_img[1];
            }
        }
        return point_img;

    }


    vector<Point> Park::point_Sort(vector<Point> squar)
    {
        //经验证，点顺序顺时针递增
        vector<Point> squar_sort;
        double edgeSet[4] = {0};
        int minyIndex = 0;
        for (int j = 0; j < 4; j++) {
            edgeSet[j] = (squar[j].x - squar[(j + 1) % 4].x) *
                         (squar[j].x - squar[(j + 1) % 4].x) +
                         (squar[j].y - squar[(j + 1) % 4].y) *
                         (squar[j].y - squar[(j + 1) % 4].y);
            if ((pow(squar[j].x,2)+pow(squar[j].y,2))< (pow(squar[minyIndex].x,2)+pow(squar[minyIndex].y,2))) minyIndex = j;
        }
        double preEdge, nxtEdge;
        nxtEdge = (squar[minyIndex].x - squar[(minyIndex + 1) % 4].x) *
                  (squar[minyIndex].x - squar[(minyIndex + 1) % 4].x) +
                  (squar[minyIndex].y - squar[(minyIndex + 1) % 4].y) *
                  (squar[minyIndex].y - squar[(minyIndex + 1) % 4].y);
        preEdge = (squar[minyIndex].x - squar[(minyIndex + 3) % 4].x) *
                  (squar[minyIndex].x - squar[(minyIndex + 3) % 4].x) +
                  (squar[minyIndex].y - squar[(minyIndex + 3) % 4].y) *
                  (squar[minyIndex].y - squar[(minyIndex + 3) % 4].y);
        if(nxtEdge>preEdge)
        {
            squar_sort.clear();
            squar_sort.emplace_back(squar[(minyIndex + 1) % 4]);
            squar_sort.emplace_back(squar[minyIndex]);
            squar_sort.emplace_back(squar[(minyIndex + 3) % 4]);
            squar_sort.emplace_back(squar[(minyIndex + 2) % 4]);
        }
        else if((nxtEdge<preEdge))
        {
            squar_sort.clear();
            squar_sort.emplace_back(squar[(minyIndex + 2) % 4]);
            squar_sort.emplace_back(squar[(minyIndex + 1) % 4]);
            squar_sort.emplace_back(squar[minyIndex]);
            squar_sort.emplace_back(squar[(minyIndex + 3) % 4]);
        }
        return squar_sort;
    }

    Point2f Park::search_aim(vector<Point2f> Point)
    {
	 
   	    std::sort(Point.begin(),Point.begin()+Point.size(),my_compare);
     
	  Point2f temp=(Point[0]+Point[1])/2;
        return temp;
    }

    bool Park::alignment(cv::Mat &srcImage,Color color_,int number,int flag_sun)
    {

#ifndef HSV_DEBUG
     set_color(color_,flag_sun));
#endif
     number_choose = number;
        send_color_ = number+1;
	//Mat src = srcImage(Range(0,srcImage.rows/2),Range(0,srcImage.cols));
        Mat src = srcImage.clone();
        get_newcandidate(src);
        vector<Point2f> point_img_;
	point_img_.clear();
	point_img_ = pick_point_aim();
	if(point_img_.empty())
        {
            transform_->tran_error();
            return false;
        }
        Point2f center = search_aim(point_img_);
	if(center.y>240)
	{
	return false;
	}
	transform_->tran_aim(center,send_color_);

for (int i = 0; i < point_img_.size(); i++)
        {
            line(src, point_img_[i % 4], point_img_[(i + 1) % 4], Scalar(0, 0, 0), 2, CV_AA);
//            string str0 = "0";
//            cv::putText(src, str0, point_img[0], cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0, 255,0 ), 2);
//            string str1 = "1";
//            cv::putText(src, str1, point_img[1], cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0, 255,0 ), 2);
//            string str2 = "2";
//            cv::putText(src, str2, point_img[2], cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0, 255,0 ), 2);
//            string str3 = "3";
//            cv::putText(src, str3, point_img[3], cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0, 255,0 ), 2);
        }
        //cv::putText(src center.x, point_img[3], cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0, 255,0 ), 2);
       // circle(src,center,5,cv::Scalar(0, 0,255 ));
#ifdef HSV_DEBUG
        namedWindow("src",0);
        imshow("src",src);
        waitKey(10);
#endif
        return true;
    }


    cv::Mat Park::getFrame()
    {

    }

    bool Park::my_compare(Point2f point1, Point2f point2)
    {
        if(point1.y <= point2.y)
            return true;
        else if(point1.y > point2.y)
            return false;

    }
}
