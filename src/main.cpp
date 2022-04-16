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
/// ChArUco를 이용하여 카메라 캘리브레이션을 수행합니다.
/// </summary>
/// <param name="stream">영상이 들어올 채널</param>
/// <param name="dict">사용할 ArUco 집합</param>
/// <param name="cameraMatrixOutput">카메라 파라미터 cv::Mat</param>
/// <param name="distCoeffsOutput">카메라 파라미터 cv::Mat</param>
void calibrate(cv::VideoCapture& stream, const cv::Ptr<cv::aruco::Dictionary>& dict, cv::Mat& cameraMatrixOutput, cv::Mat& distCoeffsOutput);
/// <summary>
/// 영상에서 ArUco Marker를 찾아 포즈를 표시합니다.
/// </summary>
/// <param name="stream">영상이 들어올 채널</param>
/// <param name="dict">사용할 ArUco 집합</param>
/// <param name="cameraMatrix">구한 카메라 파라미터 1</param>
/// <param name="distCoeffs">구한 카메라 파라미터 2</param>
void detectNShowArUco(cv::VideoCapture& stream, const cv::Ptr<cv::aruco::Dictionary>& dict, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs);

int main() {
	cv::VideoCapture webcamStream(0);

	cv::Mat img;
	cv::Mat cameraMatrix, distCoeffs;

	cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

	calibrate(webcamStream, dict, cameraMatrix, distCoeffs);
	detectNShowArUco(webcamStream, dict, cameraMatrix, distCoeffs);

	// (추가목표) 그래픽스 카메라 구성, 프레임버퍼로 그림
	// (추가목표) aruco 위치와 포즈 기준으로 모델행렬 구성, 모델 렌더링
	window = onart::createWindow("CG", 1280, 720);

}
