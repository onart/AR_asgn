#ifndef _DEBUG
	#pragma comment(lib,"opencv2/lib/opencv_imgcodecs455.lib")
	#pragma comment(lib,"opencv2/lib/opencv_highgui455.lib")
	#pragma comment(lib, "opencv2/lib/opencv_core455.lib")
	#pragma comment(lib, "opencv2/lib/opencv_videoio455.lib")
	#pragma comment(lib, "opencv2/lib/opencv_aruco455.lib")
	#pragma comment(lib, "opencv2/lib/opencv_calib3d455.lib")
#else
	#pragma comment(lib,"opencv2/lib/opencv_imgcodecs455d.lib")
	#pragma comment(lib,"opencv2/lib/opencv_highgui455d.lib")
	#pragma comment(lib, "opencv2/lib/opencv_core455d.lib")
	#pragma comment(lib, "opencv2/lib/opencv_videoio455d.lib")
	#pragma comment(lib, "opencv2/lib/opencv_aruco455d.lib")
	#pragma comment(lib, "opencv2/lib/opencv_calib3d455d.lib")
#endif
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/aruco/charuco.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"
#include <cstdio>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

#include "oagle.h"

GLFWwindow* window;

/// <summary>
/// ChArUco�� �̿��Ͽ� ī�޶� Ķ���극�̼��� �����մϴ�.
/// </summary>
/// <param name="stream">������ ���� ä��</param>
/// <param name="dict">����� ArUco ����</param>
/// <param name="cameraMatrixOutput">ī�޶� �Ķ���� cv::Mat</param>
/// <param name="distCoeffsOutput">ī�޶� �Ķ���� cv::Mat</param>
void calibrate(cv::VideoCapture& stream, const cv::Ptr<cv::aruco::Dictionary>& dict, cv::Mat& cameraMatrixOutput, cv::Mat& distCoeffsOutput);
/// <summary>
/// ���󿡼� ArUco Marker�� ã�� ��� ǥ���մϴ�.
/// </summary>
/// <param name="stream">������ ���� ä��</param>
/// <param name="dict">����� ArUco ����</param>
/// <param name="cameraMatrix">���� ī�޶� �Ķ���� 1</param>
/// <param name="distCoeffs">���� ī�޶� �Ķ���� 2</param>
void detectNShowArUco(cv::VideoCapture& stream, const cv::Ptr<cv::aruco::Dictionary>& dict, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs);

int main() {
	cv::VideoCapture webcamStream(0);

	cv::Mat img;
	cv::Mat cameraMatrix, distCoeffs;

	cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

	calibrate(webcamStream, dict, cameraMatrix, distCoeffs);
	detectNShowArUco(webcamStream, dict, cameraMatrix, distCoeffs);

	// (�߰���ǥ) �׷��Ƚ� ī�޶� ����, �����ӹ��۷� �׸�
	// (�߰���ǥ) aruco ��ġ�� ���� �������� ����� ����, �� ������
	window = onart::createWindow("CG", 1280, 720);

}
