#include <opencv2\stitching.hpp>
#include <opencv2\opencv.hpp>
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

int main()
{
	cv::VideoCapture cap_wide("wide.avi");
	cv::VideoCapture cap_long("long.avi");
	cv::Mat img_wide, img_long, img;
	cv::Stitcher stitcher = cv::Stitcher::createDefault();
	
	char a[20];

	while (cap_wide.isOpened() && cap_long.isOpened())
	{
		cap_wide >> img_wide;
		cap_long >> img_long;

		std::vector<cv::Mat> imgs;
		imgs.push_back(img_wide);
		imgs.push_back(img_long);

		sprintf(a, "FPS:%d", fps());
		putText(img, a, cv::Point(0, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255));
		cv::imshow("wide", img_wide);
		cv::imshow("long", img_long);
		if (stitcher.stitch(imgs, img) == cv::Stitcher::OK)
		{
			cv::imshow("matched", img);
		}
		int opt = cv::waitKey(10);
		switch (opt)
		{
		case 'q':
			return 1;
		case ' ':
			cv::waitKey(0);
			break;
		default:
			break;
		}
	}
}