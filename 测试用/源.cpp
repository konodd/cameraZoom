#include <opencv2\opencv.hpp>

int main(int argc, char **argv)
{
	cv::VideoCapture cap_wide(1);
	cv::VideoCapture cap_long(2);
	cv::VideoWriter write_wide("wide.avi", cv::VideoWriter::fourcc('D', 'I', 'V', 'X'), 30.0, cv::Size(640, 480), true);
	cv::VideoWriter write_long("long.avi", cv::VideoWriter::fourcc('D', 'I', 'V', 'X'), 30.0, cv::Size(640, 480), true);
	cv::Mat img_wide, img_long, img;
	cv::Rect center_box(280, 210, 80, 60);
	double i = 1.0;
	bool is_record = false;

	while (cap_wide.isOpened() && cap_long.isOpened())
	{
		cap_wide >> img_wide;
		cap_long >> img_long;
		if (img_wide.empty() || img_long.empty())
		{
			continue;
		}

		if (is_record)
		{
			write_long << img_long;
			write_wide << img_wide;
		}

		/*img = img_wide(center_box);
		cv::resize(img, img, cv::Size(center_box.width * i, center_box.height * i));*/
		cv::imshow("wide", img_wide);
		cv::imshow("long", img_long);
		//cv::imshow("center", img);
		int opt = cv::waitKey(10);
		switch (opt)
		{
		case 'q':
			return 1;
		case ' ':
			printf("¿ªÊ¼Â¼ÖÆ\n");
			is_record = true;
			break;
		/*case 'w':
			i += 0.2;
			break;
		case 's':
			i -= 0.2;
			break;*/
		default:
			break;
		}
	}
	return 0;
}