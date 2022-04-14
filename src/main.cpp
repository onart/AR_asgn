#pragma comment(lib,"opencv2/lib/opencv_imgcodecs455.lib")
#pragma comment(lib,"opencv2/lib/opencv_highgui455.lib")
#pragma comment(lib, "opencv2/lib/opencv_core455.lib")
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include <cstdio>

int main() {
	cv::Mat img = cv::imread("./singlemarkersoriginal.png");
	
	cv::imshow("helo", img);
	cv::waitKey();
}