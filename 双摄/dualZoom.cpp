#include "front_vision.hpp"
#include <string>
#include <vector>

int WIDTH_THRESHOLD = 28;
int HEIGHT_THRESHOLD = 28;
cv::Size face_size(480, 360);

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

int main(int argc, char **argv)
{
	visionToolbox vtb = visionToolbox();                                           //视觉工具箱
	cv::VideoCapture cap_wide(1);                                                  //广角摄像头
	cv::VideoCapture cap_long(2);                                                  //长焦摄像头
	std::vector<cv::Rect> face_boxes;                                              //人脸框
	cv::Rect tracking_box;                                                         //跟踪目标
	cv::Rect show_box;
	cv::Mat frame_wide;
	cv::Mat frame_long;
	cv::Mat frame_face;

	/*visionToolbox vtb2 = visionToolbox();
	std::vector<cv::Rect> face_boxes2;
	cv::Rect tracking_box2;*/

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

		if (!is_detected)
		{
			vtb.faceDetect(frame_wide, face_boxes);
			//vtb2.faceDetect(frame_long, face_boxes2);
			if (!face_boxes.empty())
			{
				is_detected = true;
				tracking_box = face_boxes[0];
				vtb.trackingKCF(frame_wide, tracking_box, false);
				//tracking_box2 = face_boxes2[0];
				//cv::rectangle(frame_wide, tracking_box, cv::Scalar(0, 255, 0));
				//cv::rectangle(frame_long, tracking_box2, cv::Scalar(0, 255, 0));
				/*cv::circle(frame_long, cv::Point(320, 240), 1, cv::Scalar(0, 0, 255));
				cv::circle(frame_long, cv::Point(tracking_box2.x + tracking_box2.width / 2, tracking_box2.y + tracking_box2.height / 2), 1, cv::Scalar(255, 0, 0));*/
			}
		}
		else
		{
			try
			{
				vtb.trackingKCF(frame_wide, tracking_box);
				if (tracking_box.width >= WIDTH_THRESHOLD || tracking_box.height >= HEIGHT_THRESHOLD)
				{
					show_box.width = tracking_box.width * 2;
					show_box.height = tracking_box.height * 1.5;
					//右边界
					if (tracking_box.x + tracking_box.width * 1.5 > frame_wide.cols)
					{
						show_box.x = frame_wide.cols - show_box.width;
					}
					//左边界
					else if (tracking_box.x - tracking_box.width * 0.5 < 0)
					{
						show_box.x = 0;
					}
					else
					{
						show_box.x = tracking_box.x - tracking_box.width / 2;
					}
					//下边界
					if (tracking_box.y + tracking_box.height * 1.25 > frame_wide.rows)
					{
						show_box.y = frame_wide.rows - show_box.height;
					}
					//上边界
					else if (tracking_box.y - tracking_box.height * 0.25 < 0)
					{
						show_box.y = 0;
					}
					else
					{
						show_box.y = tracking_box.y - tracking_box.height / 4;
					}
					
					frame_face = frame_wide(show_box);
				}
				else
				{
					frame_face = frame_long.clone();
				}
				cv::resize(frame_face, frame_face, face_size);
				cv::imshow("face", frame_face);
				cv::rectangle(frame_wide, tracking_box, cv::Scalar(0, 255, 0));
			}
			catch (const std::exception&)
			{
				is_detected = false;
			}
		}

		//显示图片及按键操作
		sprintf(str, "FPS:%d", fps());
		cv::putText(frame_wide, str, cv::Point(540, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0));
		cv::imshow("wide", frame_wide);
		cv::imshow("long", frame_long);
		int opt = cv::waitKey(10);
		switch (opt)
		{
		case ' ':                                //重新检测
			is_detected = false;
			break;
		case 'p':
			//printf("width:%d, height:%d\n", tracking_box.width, tracking_box.height);
			/*printf("wide_center(%d, %d)  long_center(%d, %d)\n", tracking_box.x + tracking_box.width / 2, tracking_box.y + tracking_box.height / 2,
				 tracking_box2.x + tracking_box2.width / 2, tracking_box2.y + tracking_box2.height / 2);*/
			break;
		case 'q':
			return 1;
		default:
			break;
		}
	}
	return 0;
}