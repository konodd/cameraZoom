///�˶������⡪��֡�  
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
	if ((curTime - lastTime) / cv::getTickFrequency() > 1.0) // ȡ�̶�ʱ����Ϊ1��
	{
		fps = frameCount;
		frameCount = 0;
		lastTime = curTime;
	}
	return fps;
}

//�˶������⺯������  
Mat MoveDetect(Mat temp, Mat frame)
{
	Mat result = frame.clone();
	//1.��background��frameתΪ�Ҷ�ͼ  
	Mat gray1, gray2;
	cvtColor(temp, gray1, CV_BGR2GRAY);
	cvtColor(frame, gray2, CV_BGR2GRAY);
	//2.��background��frame����  
	Mat diff;
	absdiff(gray1, gray2, diff);
	//imshow("diff", diff);
	//3.�Բ�ֵͼdiff_thresh������ֵ������  
	Mat diff_thresh;
	threshold(diff, diff_thresh, 50, 255, CV_THRESH_BINARY);
	//imshow("diff_thresh", diff_thresh);
	//4.��ʴ  
	Mat kernel_erode = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat kernel_dilate = getStructuringElement(MORPH_RECT, Size(18, 18));
	erode(diff_thresh, diff_thresh, kernel_erode);
	//imshow("erode", diff_thresh);
	//5.����  
	dilate(diff_thresh, diff_thresh, kernel_dilate);
	//imshow("dilate", diff_thresh);
	//6.������������������  
	vector<vector<Point> > contours;
	findContours(diff_thresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	drawContours(result, contours, -1, Scalar(0, 0, 255), 2);//��result�ϻ�������  
															 //7.��������Ӿ���  
	vector<Rect> boundRect(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] = boundingRect(contours[i]);
		rectangle(result, boundRect[i], Scalar(0, 255, 0), 2);//��result�ϻ�������Ӿ���  
	}
	return result;//����result  
}



int main()
{

	VideoCapture video(0);
	Mat frame;//�洢֡  
	Mat temp;//�洢ǰһ֡ͼ��  
	Mat result;//�洢���ͼ��  
	char a[20];
	bool is_first = true;
	if (!video.isOpened())
		return -1;
	while (1)
	{
		video >> frame;//��֡��frame  
		//imshow("frame", frame);
		if (frame.empty())//��֡�����쳣���  
		{
			cout << "frame is empty!" << endl;
			break;
		}
		if (is_first)//���Ϊ��һ֡��temp��Ϊ�գ�  
		{
			result = MoveDetect(frame, frame);//����MoveDetect()�����˶������⣬����ֵ����result  
			is_first = false;
		}
		else//�����ǵ�һ֡��temp��ֵ�ˣ�  
		{
			result = MoveDetect(temp, frame);//����MoveDetect()�����˶������⣬����ֵ����result  
		}
		sprintf(a, "FPS:%d", fps());
		putText(result, a, cv::Point(500, 30), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255));
		imshow("result", result);
		if (waitKey(10) == 27)//��ԭFPS��ʾ  
		{
			break;
		}
		temp = frame.clone();
	}
	return 0;

}