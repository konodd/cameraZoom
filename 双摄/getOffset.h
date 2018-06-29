#pragma once
#include <opencv2\opencv.hpp>
#include <fstream>
#include <vector>

void getDualOffset(cv::Mat img_wide, cv::Mat img_long)
{
	try
	{
		cv::Mat img_wide_gray, img_long_gray;
		std::vector<cv::Vec3f> circles_wide, circles_long;
		std::ofstream out("offset.txt");
		//霍夫圆检测
		cv::cvtColor(img_wide, img_wide_gray, cv::COLOR_BGR2GRAY);
		cv::cvtColor(img_long, img_long_gray, cv::COLOR_BGR2GRAY);
		cv::GaussianBlur(img_wide_gray, img_wide_gray, cv::Size(9, 9), 2, 2);
		cv::GaussianBlur(img_long_gray, img_long_gray, cv::Size(9, 9), 2, 2);
		cv::HoughCircles(img_wide_gray, circles_wide, cv::HOUGH_GRADIENT, 1, img_wide_gray.rows / 8, 60, 40);
		cv::HoughCircles(img_long_gray, circles_long, cv::HOUGH_GRADIENT, 1, img_long_gray.rows / 8, 60, 40);
		//记录圆心在两图之间的坐标差
		if (!circles_wide.empty() && !circles_long.empty())
		{
			cv::circle(img_wide, cv::Point(circles_wide[0][0], circles_wide[0][1]), circles_wide[0][2], cv::Scalar(0, 0, 255), 3);
			cv::circle(img_long, cv::Point(circles_long[0][0], circles_long[0][1]), circles_long[0][2], cv::Scalar(0, 0, 255), 3);
			if (out.is_open())
			{
				out << circles_long[0][0] - circles_wide[0][0] << " " << circles_long[0][1] - circles_wide[0][1] << std::endl;
			}
		}
		out.close();
	}
	catch (const std::exception&)
	{
		printf("Exception in getOffset module!\n");
	}
}