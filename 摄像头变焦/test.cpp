#include <opencv2\opencv.hpp>
#include <opencv2\tracking\tracker.hpp>
#include <string>
#include <vector>

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
	cv::Mat result;
	cv::Ptr<cv::TrackerKCF> face_tracker;                                          //KCF跟踪
	cv::TrackerKCF::Params face_tracker_params;                                    //KCF跟踪参数
	face_tracker_params.detect_thresh = 0.2;                                       //KCF跟踪置信度
	face_tracker_params.resize = true;
	bool isCenter = false, autoTrack = false;
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
				face_tracker = cv::TrackerKCF::create(face_tracker_params);
				face_tracker->init(frame, tracking_box);
				is_detected = true;
			}
		}
		//跟踪
		else
		{
			if (face_tracker->update(frame, tracking_box))
			{
				result = frame(tracking_box);
				cv::resize(result, result, cv::Size(tracking_box.width * 1.5, tracking_box.height * 1.5));
				cv::imshow("face", result);
				cv::putText(frame, "tracking face", cv::Point(0, 20), cv::FONT_HERSHEY_COMPLEX, 0.6, cv::Scalar(255, 0, 0));
				cv::rectangle(frame, tracking_box, cv::Scalar(0, 0, 255));
				cv::putText(frame, "tracking", cv::Point(tracking_box.x, tracking_box.y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
			}
			else
			{
				face_tracker.release();
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
		case 'q':
			return 1;
		default:
			break;
		}
	}
	return 0;
}