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
#include<iostream>

int main() {
	cv::VideoCapture webcamStream(0);

	cv::Mat img;
	cv::Size imgSize;
	cv::Mat cameraMatrix, distCoeffs;
	std::vector<cv::Mat> rvecs, tvecs;

	cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	std::vector<int> markerIds;
	std::vector<std::vector<cv::Point2f>> markerCorners;

	//std::vector<std::vector<cv::Point2f>> rejecteds;
	
	cv::aruco::DetectorParameters params;
	//cv::Ptr<cv::aruco::GridBoard> gridboard = cv::aruco::GridBoard::create(7, 5, 0.021f, 0.007f, dict, 0);
	//cv::Ptr<cv::aruco::Board> board = gridboard.staticCast<cv::aruco::Board>();

	cv::Ptr<cv::aruco::CharucoBoard> charucoboard = cv::aruco::CharucoBoard::create(5, 7, 21.0f, 11.0f, dict);
	cv::Ptr<cv::aruco::Board> board = charucoboard.staticCast<cv::aruco::Board>();

	std::vector< std::vector< cv::Point2f > > allCorners;
	std::vector< int > allIds;
	std::vector< int > counter;

	std::vector< cv::Mat > allCharucoCorners;
	std::vector< cv::Mat > allCharucoIds;
	std::vector< cv::Mat > filteredImages;

	printf("press c key to capture a frame containing ArUco for calibration\nIf done enough, press enter to proceed\n");
	while(1) {	// webcam 찾아서 스트림(실시간 img 읽어오기+쓰기)
		int key;
		if (!webcamStream.read(img)) {
			fprintf(stderr, "No video input device.\n");
			break;
		}
		// aruco 감지
		cv::aruco::detectMarkers(img, dict, markerCorners, markerIds);

		switch (key=cv::waitKey(17))
		{
		case 'C':
		case 'c':	// 카메라 보정
			if (markerIds.size()) {
				allCorners.insert(allCorners.cend(), markerCorners.cbegin(), markerCorners.cend());
				allIds.insert(allIds.cend(), markerIds.cbegin(), markerIds.cend());
				counter.push_back((int)markerCorners.size());
				imgSize = img.size();
				cv::Mat currentCharucoCorners, currentCharucoIds;
				cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, img, charucoboard, currentCharucoCorners, currentCharucoIds);
				allCharucoCorners.push_back(currentCharucoCorners);
				allCharucoIds.push_back(currentCharucoIds);
				//filteredImages.push_back(img);
				printf("captured %lld markers\n", markerIds.size());

			}
			break;
		case 13:
			printf("done\n");
			break;
		default:
			break;
		}
		if (key == 13 && allIds.size()) break;
		cv::aruco::drawDetectedMarkers(img, markerCorners, markerIds);
		cv::imshow("helo", img);
	};

	double repError = cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, charucoboard, imgSize, cameraMatrix, distCoeffs);
	//double err = cv::aruco::calibrateCameraAruco(allCorners, allIds, counter, board, imgSize, cameraMatrix, distCoeffs, rvecs, tvecs);
	//printf("reprojection error=%.2f\n", repError);
	std::cout << cameraMatrix << "\n\n";

	while (1) {
		int key;
		if (!webcamStream.read(img)) {
			fprintf(stderr, "No video input device.\n");
			break;
		}
		cv::aruco::detectMarkers(img, dict, markerCorners, markerIds);
		cv::aruco::drawDetectedMarkers(img, markerCorners, markerIds);
		std::vector<cv::Vec3d> rvs, tvs;
		cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.021f, cameraMatrix, distCoeffs, rvs, tvs);
		for (size_t i = 0; i < rvs.size(); i++) {
			cv::drawFrameAxes(img, cameraMatrix, distCoeffs, rvs[i], tvs[i], 0.05f, 1);
		}
		
		cv::imshow("helo", img);
		key = cv::waitKey(17);
		if (key == 13) break;
	}

	// aruco 감지/포즈 판별 및 화면상 출력
	// (추가목표) 그래픽스 카메라 구성, 프레임버퍼로 그림
	// (추가목표) aruco 위치와 포즈 기준으로 모델행렬 구성, 모델 렌더링

}
