#pragma once
#include <opencv2\opencv.hpp>
#include <vector>

void getDualOffset(cv::Mat &frame_wide, cv::Mat &frame_long)
{
	try
	{
		cv::Mat frame_wide_gray, frame_long_gray;
		std::vector<cv::Vec3f> circles_wide, circles_long;
		int x_offset, int y_offset;
		//霍夫圆检测
		cv::cvtColor(frame_wide, frame_wide_gray, cv::COLOR_BGR2GRAY);
		cv::cvtColor(frame_long, frame_long_gray, cv::COLOR_BGR2GRAY);
		cv::GaussianBlur(frame_wide_gray, frame_wide_gray, cv::Size(9, 9), 2, 2);
		cv::GaussianBlur(frame_long_gray, frame_long_gray, cv::Size(9, 9), 2, 2);
		cv::HoughCircles(frame_wide_gray, circles_wide, cv::HOUGH_GRADIENT, 1, frame_wide_gray.rows / 8, 60, 40);
		cv::HoughCircles(frame_long_gray, circles_long, cv::HOUGH_GRADIENT, 1, frame_long_gray.rows / 8, 60, 40);
		if (!circles_long.empty())
		{
			cv::circle(frame_long, cv::Point(circles_long[0][0], circles_long[0][1]), circles_long[0][2], cv::Scalar(0, 0, 255), 3);
		}
		if (!circles_wide.empty())
		{
			cv::circle(frame_wide, cv::Point(circles_wide[0][0], circles_wide[0][1]), circles_wide[0][2], cv::Scalar(0, 0, 255), 3);
		}
		//记录圆心在两图之间的坐标差
		if (!circles_wide.empty() && !circles_long.empty())
		{
			x_offset = circles_long[0][0] - circles_wide[0][0];
			y_offset = circles_long[0][1] - circles_wide[0][1];
			printf("x_offset = %d, y_offset = &d", x_offset, y_offset);
		}
	}
	catch (const std::exception&)
	{
		printf("Exception in getOffset module!\n");
	}
}