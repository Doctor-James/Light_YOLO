#ifndef PARK_H
#define PARK_H

#include<opencv2/opencv.hpp>
#include "camera.h"
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <opencv2/core/eigen.hpp>
#include "sophus/se3.h"
#include "sophus/so3.h"
#include "rotate.h"
#include "rvec2elur.h"
#include "transform.h"

using namespace cv;
using namespace std;
enum Color
{
    yellow,
    green,
    blue,
    red,
    gray
};

struct math_center
{
    Point2f point_;
    int number;
};
namespace zjl
{
    class Park
    {
        friend class Autodrive;
    public:
        Park(serialPortReadWriteThread *serialPortReadWriteThread);
        bool initial();
        bool getpark(cv::Mat &src);
        bool alignment(cv::Mat &src,Color color_,int number,int flag_sun);
        void set_color(Color color,int flag_sun);
        void get_newcandidate(cv::Mat &src);
        void get_newcandidate_rgb(cv::Mat &src);
        Point2f search_aim(vector<Point2f> Point);
        static bool my_compare(Point2f point1, Point2f point2);
        vector<Point> point_Sort(vector<Point> squar);
        cv::Mat getFrame();
        vector<Point2f> pick_point();
        vector<Point2f> pick_point_aim();
        bool solve_pnp(vector<Point2f> point_img);
        cv::VideoCapture cap_;
        cv::Mat frame;
        Mat srcImage, imgHSV, imgThresholded;
        int iLowH , iLowS , iLowV , iHighH , iHighS , iHighV ;
    private:
        struct timeval start, end;
	int number_choose;
        Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
        Mat distCoeffs = Mat(5, 1, CV_32FC1, Scalar::all(0));
        serialPortReadWriteThread *serialPortReadWriteThread_;
        transform *transform_;

        vector<math_center>Point_red_;
        vector<math_center>Point_blue_;
        cv::Mat CannyImage;
        vector<Point3d> _Point3D;
        vector<vector<Point>> _contours;
        vector<int> _squar_index;
        Color color_;
        double _Cx, _Cy, _Cz;
        Sophus::SE3 TF;
        cv::Mat rvec = cv::Mat::zeros(3, 1, CV_64F);
        cv::Mat tvec = cv::Mat::zeros(3, 1, CV_64F);
	int send_color_=0;
    };

    //车位结构体
    typedef struct Stall
    {
        float lengthWidthRatio;//长宽比,1:1--->0;6:1--->1
        float angle;//角度
        float area;//面积 200
        float length;
        float width;
        cv::Point2f point[4];
        cv::Point2f center;
    }Stall;

}
#endif
