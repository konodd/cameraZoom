#include "front_vision.hpp"
#include "getOffset.h"
#include <string>
#include <vector>

int WIDTH_THRESHOLD;
int HEIGHT_THRESHOLD;

int fps()
{
	static int fps = 0;
	static int lastTime = cv::getTickCount();
	static int frameCount = 0;

	++frameCount;

	int curTime = cv::getTickCount();
	if ((curTime - lastTime) / cv::getTickFrequency() > 1.0) // 取固定时间间隔为1秒
	{
		fps = frameCount;
		frameCount = 0;
		lastTime = curTime;
	}
	return fps;
}

int _main(int argc, char **argv)
{
	visionToolbox vtb = visionToolbox();                                           //视觉工具箱
	cv::VideoCapture cap_wide(1);                                                  //广角摄像头
	cv::VideoCapture cap_long(2);                                                  //长焦摄像头
	std::vector<cv::Rect> face_boxes;                                              //人脸框
	cv::Rect tracking_box;                                                         //跟踪目标
	cv::Mat frame_wide;
	cv::Mat frame_long;
	cv::Mat frame_face;
	int x_offset = 0;
	int y_offset = 0;

	char str[20];
	bool is_detected = false;

	while (cap_wide.isOpened() && cap_long.isOpened())
	{
		cap_wide >> frame_wide;
		cap_long >> frame_long;
		if (frame_wide.empty() && frame_long.empty())
		{
			continue;
		}
		
		getDualOffset(frame_wide, frame_long, x_offset, y_offset);
		
		//显示图片及按键操作
		//sprintf(str, "FPS:%d", fps());
		//cv::putText(frame_wide, str, cv::Point(540, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0));
		cv::imshow("wide", frame_wide);
		cv::imshow("long", frame_long);
		int opt = cv::waitKey(10);
		switch (opt)
		{
		case 'p':                                //暂停
			cv::waitKey(0);
			break;
		case ' ':                                //重新检测
			printf("x_offset = %d, y_offset = %d\n", x_offset, y_offset);
			break;
		case 'q':
			return 1;
		default:
			break;
		}
	}
	return 0;
}