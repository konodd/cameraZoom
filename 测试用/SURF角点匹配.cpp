//#include <opencv2\highgui\highgui.hpp>
//#include <opencv2\features2d\features2d.hpp>
#include <opencv2\xfeatures2d\nonfree.hpp>
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
	cv::Mat img_wide, img_long;
	cv::Mat img_wide_dst, img_long_dst;
	cv::Mat img_wide_keys, img_long_keys;
	cv::Mat img_match;
	cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create();
	cv::BFMatcher matcher;
	std::vector<cv::KeyPoint> wide_keys, long_keys;
	char a[20];

	while (cap_wide.isOpened() && cap_long.isOpened())
	{
		cap_wide >> img_wide;
		cap_long >> img_long;
		
		surf->detectAndCompute(img_wide, cv::Mat(), wide_keys, img_wide_dst);
		surf->detectAndCompute(img_long, cv::Mat(), long_keys, img_long_dst);
		std::vector<cv::DMatch> matches;
		matcher.match(img_long_dst, img_wide_dst, matches);
		std::sort(matches.begin(), matches.end());
		std::vector<cv::DMatch> good_matches;
		int ptsPairs = std::min(50, (int)(matches.size() * 0.15));
		for (int i = 0; i < ptsPairs; i++)
		{
			good_matches.push_back(matches[i]);
		}
		cv::drawMatches(img_long, long_keys, img_wide, wide_keys, good_matches, img_match);
		std::vector<cv::Point2f> obj;
		std::vector<cv::Point2f> scene;

		for (size_t i = 0; i < good_matches.size(); i++)
		{
			obj.push_back(wide_keys[good_matches[i].queryIdx].pt);
			scene.push_back(long_keys[good_matches[i].trainIdx].pt);
		}

		std::vector<cv::Point2f> obj_corners(4), scene_corners(4);
		obj_corners[0] = cv::Point(0, 0);
		obj_corners[1] = cv::Point(img_wide.cols, 0);
		obj_corners[2] = cv::Point(img_wide.cols, img_wide.rows);
		obj_corners[3] = cv::Point(0, img_wide.rows);

		cv::Mat H = cv::findHomography(obj, scene, cv::RANSAC);
		cv::perspectiveTransform(obj_corners, scene_corners, H);

		cv::line(img_match, scene_corners[0] + cv::Point2f((float)img_wide.cols, 0), scene_corners[1] + cv::Point2f((float)img_wide.cols, 0), cv::Scalar(0, 255, 0), 2, cv::LINE_AA);       //绘制
		cv::line(img_match, scene_corners[1] + cv::Point2f((float)img_wide.cols, 0), scene_corners[2] + cv::Point2f((float)img_wide.cols, 0), cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
		cv::line(img_match, scene_corners[2] + cv::Point2f((float)img_wide.cols, 0), scene_corners[3] + cv::Point2f((float)img_wide.cols, 0), cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
		cv::line(img_match, scene_corners[3] + cv::Point2f((float)img_wide.cols, 0), scene_corners[0] + cv::Point2f((float)img_wide.cols, 0), cv::Scalar(0, 255, 0), 2, cv::LINE_AA);

		sprintf(a, "FPS:%d", fps());
		putText(img_match, a, cv::Point(0, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255));
		cv::imshow("match", img_match);
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