#include<opencv2\opencv.hpp>

int main(int argc, char **argv)
{
	cv::VideoCapture cap_wide_angle(0);
	cv::VideoCapture cap_telephoto(1);
	cv::Mat frame_wide_angle, frame_telephoto;
	while (cap_wide_angle.isOpened() && cap_telephoto.isOpened())
	{
		cap_wide_angle >> frame_wide_angle;
		cap_telephoto >> frame_telephoto;
		cv::imshow("wide_angle", frame_wide_angle);
		cv::imshow("telephoto", frame_telephoto);
		if (cv::waitKey(10) == 'q')
		{
			break;
		}
	}
	return 0;
}