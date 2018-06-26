#pragma once
#include <opencv2\opencv.hpp>
#include <kcftracker.hpp>

class visionToolbox
{
public:
	visionToolbox();
	~visionToolbox();
	void faceDetect(cv::Mat img, std::vector<cv::Rect> face_boxes);                          //ÈËÁ³¼ì²â
	void movingObjectDetect(cv::Mat img, std::vector<cv::Rect> object_boxes);                //ÒÆ¶¯ÎïÌå¼ì²â
	void trackingKCF(cv::Mat img, cv::Rect box);                                             //KCF¸ú×Ù

private:
	cv::CascadeClassifier cas;
	KCFTracker tracker_kcf;
};

visionToolbox::visionToolbox() {}

visionToolbox::~visionToolbox() {}

void visionToolbox::faceDetect(cv::Mat img, std::vector<cv::Rect> face_boxes)
{

}

void visionToolbox::movingObjectDetect(cv::Mat img, std::vector<cv::Rect> object_boxes)
{

}

void visionToolbox::trackingKCF(cv::Mat img, cv::Rect box)
{

}