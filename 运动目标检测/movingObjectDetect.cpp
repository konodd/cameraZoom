///运动物体检测——帧差法  
#include"opencv2/opencv.hpp"  
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;
#include <iostream>  
using namespace std;

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

//运动物体检测函数声明  
Mat MoveDetect(Mat temp, Mat frame)
{
	Mat result = frame.clone();
	//1.将background和frame转为灰度图  
	Mat gray1, gray2;
	cvtColor(temp, gray1, CV_BGR2GRAY);
	cvtColor(frame, gray2, CV_BGR2GRAY);
	//2.将background和frame做差  
	Mat diff;
	absdiff(gray1, gray2, diff);
	//imshow("diff", diff);
	//3.对差值图diff_thresh进行阈值化处理  
	Mat diff_thresh;
	threshold(diff, diff_thresh, 50, 255, CV_THRESH_BINARY);
	//imshow("diff_thresh", diff_thresh);
	//4.腐蚀  
	Mat kernel_erode = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat kernel_dilate = getStructuringElement(MORPH_RECT, Size(18, 18));
	erode(diff_thresh, diff_thresh, kernel_erode);
	//imshow("erode", diff_thresh);
	//5.膨胀  
	dilate(diff_thresh, diff_thresh, kernel_dilate);
	//imshow("dilate", diff_thresh);
	//6.查找轮廓并绘制轮廓  
	vector<vector<Point> > contours;
	findContours(diff_thresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	drawContours(result, contours, -1, Scalar(0, 0, 255), 2);//在result上绘制轮廓  
															 //7.查找正外接矩形  
	vector<Rect> boundRect(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] = boundingRect(contours[i]);
		rectangle(result, boundRect[i], Scalar(0, 255, 0), 2);//在result上绘制正外接矩形  
	}
	return result;//返回result  
}



int main()
{

	VideoCapture video(0);
	Mat frame;//存储帧  
	Mat temp;//存储前一帧图像  
	Mat result;//存储结果图像  
	char a[20];
	bool is_first = true;
	if (!video.isOpened())
		return -1;
	while (1)
	{
		video >> frame;//读帧进frame  
		//imshow("frame", frame);
		if (frame.empty())//对帧进行异常检测  
		{
			cout << "frame is empty!" << endl;
			break;
		}
		if (is_first)//如果为第一帧（temp还为空）  
		{
			result = MoveDetect(frame, frame);//调用MoveDetect()进行运动物体检测，返回值存入result  
			is_first = false;
		}
		else//若不是第一帧（temp有值了）  
		{
			result = MoveDetect(temp, frame);//调用MoveDetect()进行运动物体检测，返回值存入result  
		}
		sprintf(a, "FPS:%d", fps());
		putText(result, a, cv::Point(500, 30), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255));
		imshow("result", result);
		if (waitKey(10) == 27)//按原FPS显示  
		{
			break;
		}
		temp = frame.clone();
	}
	return 0;

}