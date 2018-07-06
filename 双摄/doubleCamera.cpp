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
	if ((curTime - lastTime) / cv::getTickFrequency() > 1.0) // ȡ�̶�ʱ����Ϊ1��
	{
		fps = frameCount;
		frameCount = 0;
		lastTime = curTime;
	}
	return fps;
}

int _main(int argc, char **argv)
{
	visionToolbox vtb = visionToolbox();                                           //�Ӿ�������
	cv::VideoCapture cap_wide(1);                                                  //�������ͷ
	cv::VideoCapture cap_long(2);                                                  //��������ͷ
	std::vector<cv::Rect> face_boxes;                                              //������
	cv::Rect tracking_box;                                                         //����Ŀ��
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
		
		//��ʾͼƬ����������
		//sprintf(str, "FPS:%d", fps());
		//cv::putText(frame_wide, str, cv::Point(540, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0));
		cv::imshow("wide", frame_wide);
		cv::imshow("long", frame_long);
		int opt = cv::waitKey(10);
		switch (opt)
		{
		case 'p':                                //��ͣ
			cv::waitKey(0);
			break;
		case ' ':                                //���¼��
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