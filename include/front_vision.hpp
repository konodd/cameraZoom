#pragma once
#include <opencv2\opencv.hpp>
#include <kcftracker.hpp>

class visionToolbox
{
public:
	visionToolbox();
	~visionToolbox();
	void circleDetect(cv::Mat img, std::vector<cv::Vec3f> &circles);                                          //霍夫圆检测
	void faceDetect(cv::Mat img, std::vector<cv::Rect> &face_boxes);                                          //人脸检测
	void movingObjectDetect(cv::Mat img_last, cv::Mat img_current, std::vector<cv::Rect> &object_boxes);      //移动物体检测
	void trackingKCF(cv::Mat img, cv::Rect &box);                                                             //KCF跟踪

private:
	cv::CascadeClassifier cas;                   //级联检测器
	KCFTracker tracker_kcf;                      //KCF跟踪器
	bool face_detect_init;                       //用于第一次执行人脸检测时的初始化
	bool tracking_KCF_init;                      //用于第一次执行人脸检测时的初始化
};

visionToolbox::visionToolbox() 
{
	face_detect_init = false;
	tracking_KCF_init = false;
}

visionToolbox::~visionToolbox() {}

void visionToolbox::circleDetect(cv::Mat img, std::vector<cv::Vec3f> &circles)
{
	try
	{
		cv::Mat img_gray;
		cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
		//霍夫圆检测，参数可调节
		cv::GaussianBlur(img_gray, img_gray, cv::Size(9, 9), 2, 2);
		cv::HoughCircles(img_gray, circles, CV_HOUGH_GRADIENT, 1, img_gray.rows / 8, 60, 40);
	}
	catch (const std::exception&)
	{
		printf("Exception in circleDetect module!\n");
	}
}

void visionToolbox::faceDetect(cv::Mat img, std::vector<cv::Rect> &face_boxes)
{
	try
	{
		if (!face_detect_init)
		{
			//可自由加载级联检测模型
			cas.load("E:/ROS/xl320_driver/haarcascades/haarcascade_frontalface_alt.xml");
			face_detect_init = true;
		}
		cas.detectMultiScale(img, face_boxes);
	}
	catch (const std::exception&)
	{
		printf("Exception in faceDetect module!\n");
	}
}

void visionToolbox::movingObjectDetect(cv::Mat img_last, cv::Mat img_current, std::vector<cv::Rect> &object_boxes)
{
	try
	{
		cv::Mat img_last_gray, img_current_gray;
		cv::Mat tmp;
		std::vector<std::vector<cv::Point>> contours;
		//转为灰度图
		cv::cvtColor(img_last, img_last_gray, cv::COLOR_BGR2GRAY);
		cv::cvtColor(img_current, img_current_gray, cv::COLOR_BGR2GRAY);
		//将上一帧和当前帧做差
		cv::absdiff(img_last_gray, img_current_gray, tmp);
		//将差值图进行阈值化处理
		cv::threshold(tmp, tmp, 50, 255, CV_THRESH_BINARY);
		//腐蚀
		cv::erode(tmp, tmp, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
		//膨胀
		cv::dilate(tmp, tmp, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(18, 18)));
		//寻找运动物体的轮廓
		cv::findContours(tmp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		//用框框出轮廓
		for (int i = 0; i < contours.size(); i++)
		{
			object_boxes[i] = cv::boundingRect(contours[i]);
		}
	}
	catch (const std::exception&)
	{
		printf("Exception in movingObjecDetect module!\n");
	}
}

void visionToolbox::trackingKCF(cv::Mat img, cv::Rect &box)
{
	try
	{
		if (!tracking_KCF_init)
		{
			tracker_kcf = KCFTracker();
			tracking_KCF_init = true;
		}
		box = tracker_kcf.update(img);
	}
	catch (const std::exception&)
	{
		printf("Exception in trackingKCF module!\n");
	}
}