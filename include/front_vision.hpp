#pragma once
#include <opencv2\opencv.hpp>
#include <kcftracker.hpp>

class visionToolbox
{
public:
	visionToolbox();
	~visionToolbox();
	void circleDetect(cv::Mat img, std::vector<cv::Vec3f> &circles);                                          //����Բ���
	void faceDetect(cv::Mat img, std::vector<cv::Rect> &face_boxes);                                          //�������
	void movingObjectDetect(cv::Mat img_last, cv::Mat img_current, std::vector<cv::Rect> &object_boxes);      //�ƶ�������
	void trackingKCF(cv::Mat img, cv::Rect &box);                                                             //KCF����

private:
	cv::CascadeClassifier cas;                   //���������
	KCFTracker tracker_kcf;                      //KCF������
	bool face_detect_init;                       //���ڵ�һ��ִ���������ʱ�ĳ�ʼ��
	bool tracking_KCF_init;                      //���ڵ�һ��ִ���������ʱ�ĳ�ʼ��
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
		//����Բ��⣬�����ɵ���
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
			//�����ɼ��ؼ������ģ��
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
		//תΪ�Ҷ�ͼ
		cv::cvtColor(img_last, img_last_gray, cv::COLOR_BGR2GRAY);
		cv::cvtColor(img_current, img_current_gray, cv::COLOR_BGR2GRAY);
		//����һ֡�͵�ǰ֡����
		cv::absdiff(img_last_gray, img_current_gray, tmp);
		//����ֵͼ������ֵ������
		cv::threshold(tmp, tmp, 50, 255, CV_THRESH_BINARY);
		//��ʴ
		cv::erode(tmp, tmp, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
		//����
		cv::dilate(tmp, tmp, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(18, 18)));
		//Ѱ���˶����������
		cv::findContours(tmp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		//�ÿ�������
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