#include <opencv2\opencv.hpp>
//#include <opencv2\tracking\tracker.hpp>
#include <string>
#include <vector>
#include <kcftracker.hpp>

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
	cv::CascadeClassifier cas("E:/ROS/xl320_driver/haarcascades/haarcascade_frontalface_alt.xml");  //级联检测
	cv::VideoCapture cap(0);
	std::vector<cv::Rect> face_boxes;                                              //人脸框
	cv::Rect2d tracking_box;                                                       //跟踪目标
	cv::Mat frame;
	cv::Mat frame_face;

	bool HOG = true;
	bool FIXEDWINDOW = false;
	bool MULTISCALE = true;
	bool LAB = false;
	KCFTracker tracker_kcf(HOG, FIXEDWINDOW, MULTISCALE, LAB);

	char str[20];
	bool is_detected = false;

	while (cap.isOpened())
	{
		cap >> frame;
		if (frame.empty())
		{
			continue;
		}
		//检测
		if (!is_detected)
		{
			cv::putText(frame, "detecting face", cv::Point(0, 20), cv::FONT_HERSHEY_COMPLEX, 0.6, cv::Scalar(255, 0, 0));
			cas.detectMultiScale(frame, face_boxes);
			//检测到目标后，初始化跟踪模块
			if (!face_boxes.empty())
			{
				tracking_box = face_boxes[0];
				tracker_kcf.init(tracking_box, frame);
				is_detected = true;
			}
		}
		//跟踪
		else
		{
			try
			{
				tracking_box = tracker_kcf.update(frame);
				frame_face = frame(tracking_box);
				cv::rectangle(frame, tracking_box, cv::Scalar(0, 0, 255));
				cv::imshow("face", frame_face);
			}
			catch (const std::exception&)
			{
				printf("tracking failed!!!\n");
				is_detected = false;
			}
		}
		//显示图片及按键操作
		sprintf(str, "FPS:%d", fps());
		cv::putText(frame, str, cv::Point(540, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0));
		cv::imshow("test", frame);
		int opt = cv::waitKey(10);
		switch (opt)
		{
		case 'p':                                //暂停
			cv::waitKey(0);
			break;
		case ' ':                                //重新检测
			is_detected = false;
			break;
		case 'q':
			return 1;
		default:
			break;
		}
	}
	return 0;
}