#include "front_vision.hpp"
#include "middle_plan.hpp"
#include "back_control.hpp"
#include <string>
#include <vector>

int WIDTH_THRESHOLD = 28;
int HEIGHT_THRESHOLD = 28;
cv::Size face_size(480, 360);

controlToolBox mydxl = controlToolBox();
const int radius_threshold = 30;

void moveDXL(int8_t id, int16_t position, int16_t velocity)
{
	mydxl.setGoalVelocity(id, velocity);
	mydxl.setGoalPosition(id, position);
}

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

void show(cv::Mat &img_dst, cv::Mat img_src, cv::Rect tracking_box)
{
	cv::Rect show_box;
	show_box.width = tracking_box.width * 2;
	show_box.height = tracking_box.height * 1.5;
	//右边界
	if (tracking_box.x + tracking_box.width * 1.5 > img_src.cols)
	{
		show_box.x = img_src.cols - show_box.width;
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
	if (tracking_box.y + tracking_box.height * 1.25 > img_src.rows)
	{
		show_box.y = img_src.rows - show_box.height;
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

	img_dst = img_src(show_box);
}

int main(int argc, char **argv)
{
	visionToolbox vtb = visionToolbox();                                           //视觉工具箱
	cv::VideoCapture cap_wide(1);                                                  //广角摄像头
	cv::VideoCapture cap_long(2);                                                  //长焦摄像头
	std::vector<cv::Rect> face_boxes;                                              //人脸框
	cv::Rect tracking_box;                                                         //跟踪目标
	cv::Mat frame_wide;
	cv::Mat frame_long;
	cv::Mat frame_face;

	visionToolbox vtb2 = visionToolbox();
	std::vector<cv::Rect> face_boxes2;
	cv::Rect tracking_box2;

	cv::Point lastPoint = cv::Point(320, 240);                                     //用于保存上一帧中的目标位置
	cv::Point center;                                                              //本帧中目标的位置
	int dxl1, dxl2;                                                                //舵机1和2的位置
	int dxl1_center, dxl2_center;                                                  //标定表中，中心坐标的舵机位置
	planToolBox sd;
	bool isCenter = false, autoTrack = false;

	char str[20];
	bool is_detected = false;

	if (mydxl.init() && sd.setDataPath("dxl_angle.txt"))
	{
		mydxl.setLED(1, 5);
		mydxl.setLED(2, 5);
		sd.getPoints();
		//init center
		sd.getPosition(320, 240);
		sd.getDXLPosition(dxl1_center, dxl2_center);
		moveDXL(1, dxl1_center, (abs(dxl1_center - mydxl.getPresentPosition(1)) / 2));
		moveDXL(2, dxl2_center, (abs(dxl2_center - mydxl.getPresentPosition(2)) / 2));

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
				vtb2.faceDetect(frame_long, face_boxes2);
				if (!face_boxes.empty())
				{
					is_detected = true;
					tracking_box = face_boxes[0];
					vtb.trackingKCF(frame_wide, tracking_box, false);
					if (!face_boxes2.empty())
					{
						tracking_box2 = face_boxes2[0];
						vtb2.trackingKCF(frame_long, tracking_box2, false);
					}
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
						show(frame_face, frame_wide, tracking_box);
					}
					else
					{
						vtb2.trackingKCF(frame_long, tracking_box2);
						show(frame_face, frame_long, tracking_box2);
					}
					cv::resize(frame_face, frame_face, face_size);
					cv::imshow("变焦效果图", frame_face);
					cv::rectangle(frame_wide, tracking_box, cv::Scalar(0, 255, 0));

					if (autoTrack)
					{
						center = cv::Point(tracking_box.x + tracking_box.width / 2, tracking_box.y + tracking_box.height / 2);
						//设置偏差范围
						int dx = center.x - 320;
						int dy = center.y - 240;
						//到达阈值范围，舵机停止运动
						if (dx * dx + dy * dy <= radius_threshold * radius_threshold && !isCenter)
						{
							moveDXL(1, mydxl.getPresentPosition(1), 40);
							moveDXL(2, mydxl.getPresentPosition(2), 40);
							isCenter = true;
						}
						else if (autoTrack && dx * dx + dy * dy > radius_threshold * radius_threshold)
						{
							isCenter = false;
							if (sd.getPosition(960 - (center.x + lastPoint.x), 720 - (center.y + lastPoint.y)))
							{
								lastPoint = center;
								sd.getDXLPosition(dxl1, dxl2);
								moveDXL(1, dxl1, (abs(dxl1 - mydxl.getPresentPosition(1)) / 2));
								moveDXL(2, dxl2, (abs(dxl2 - mydxl.getPresentPosition(2)) / 2));
							}
						}
					}
				}
				catch (const std::exception&)
				{
					is_detected = false;
				}
			}

			//显示图片及按键操作
			sprintf(str, "FPS:%d", fps());
			cv::putText(frame_wide, str, cv::Point(540, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0));
			cv::imshow("广角原图", frame_wide);
			cv::imshow("长焦原图", frame_long);
			int opt = cv::waitKey(10);
			switch (opt)
			{
			case ' ':                                //重新检测
				is_detected = false;
				break;
			case 'a':
				autoTrack = !autoTrack;
				break;
			case 'r':                                //还原到初始位置
				mydxl.setGoalTorque(1, 1023);
				mydxl.setGoalTorque(2, 1023);
				moveDXL(1, dxl1_center, 40);
				moveDXL(2, dxl2_center, 40);
				lastPoint = cv::Point(320, 240);
				break;
			case 'p':
				//printf("width:%d, height:%d\n", tracking_box.width, tracking_box.height);
				/*printf("wide_center(%d, %d)  long_center(%d, %d)\n", tracking_box.x + tracking_box.width / 2, tracking_box.y + tracking_box.height / 2,
					 tracking_box2.x + tracking_box2.width / 2, tracking_box2.y + tracking_box2.height / 2);*/
				break;
			case 'q':
				mydxl.setLED(1, 0);
				mydxl.setLED(2, 0);
				return 1;
			default:
				break;
			}
		}
	}
	return 0;
}