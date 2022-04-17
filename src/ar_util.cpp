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
#include <cstdlib>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

const float MARKER1_SIDE = 0.124f;
const float MARKERBOARD_SIDE = 0.021f;
const float MARKERGRID_GAP = 0.007f;
const float CHARUCOGRID_SIDE = 0.021f;
const float CHARUCOMARKER_SIDE = 0.011f;

void calibrate(cv::VideoCapture& stream, const cv::Ptr<cv::aruco::Dictionary>& dict, cv::Mat& cameraMatrixOutput, cv::Mat& distCoeffsOutput) {
	//cv::aruco::DetectorParameters params;
	printf("press c key to capture a frame containing ChArUco for calibration\nIf done enough, press enter to proceed\n");
	cv::Mat img;
	std::vector<int> markerIds;
	std::vector<std::vector<cv::Point2f>> markerCorners;
	cv::Size imgSize;
#define CALIBRATE_CH
#ifdef CALIBRATE_CH
	cv::Ptr<cv::aruco::CharucoBoard> charucoboard = cv::aruco::CharucoBoard::create(5, 7, CHARUCOGRID_SIDE, CHARUCOMARKER_SIDE, dict);
	cv::Ptr<cv::aruco::Board> board = charucoboard.staticCast<cv::aruco::Board>();
	std::vector< cv::Mat > allCharucoCorners;
	std::vector< cv::Mat > allCharucoIds;
#else	// 보정이 잘 되지 않아 폐기
	cv::Ptr<cv::aruco::GridBoard> gridboard = cv::aruco::GridBoard::create(7, 5, MARKERBOARD_SIDE, MARKERGRID_GAP, dict, 0);
	cv::Ptr<cv::aruco::Board> board = gridboard.staticCast<cv::aruco::Board>();
	std::vector< std::vector< cv::Point2f > > allCorners;
	std::vector< int > allIds;
	std::vector< int > counter;
#endif
	while (1) {	// webcam 찾아서 스트림(실시간 img 읽어오기+쓰기)
		int key;
		if (!stream.read(img)) {
			fprintf(stderr, "No video input device.\n");
			break;
		}
		// aruco 감지
		cv::aruco::detectMarkers(img, dict, markerCorners, markerIds);

		switch (key = cv::waitKey(17))
		{
		case 'C':
		case 'c':	// 카메라 보정
			if (markerIds.size()) {
				cv::Mat prevCamera;
				cameraMatrixOutput.copyTo(prevCamera);
				imgSize = img.size();
				system("cls;clear");
				printf("captured %lld markers\n", markerIds.size());
#ifdef CALIBRATE_CH
				if (markerIds.size() < 4) break;
				cv::Mat currentCharucoCorners, currentCharucoIds;
				cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, img, charucoboard, currentCharucoCorners, currentCharucoIds);
				if (currentCharucoCorners.total() <= 4) {
					printf("Charuco Board not detected.\n");
					continue;
				}
				allCharucoCorners.push_back(currentCharucoCorners);
				allCharucoIds.push_back(currentCharucoIds);
				double err = cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, charucoboard, imgSize, cameraMatrixOutput, distCoeffsOutput);
				printf("reprojection error=%.2f\n", err);
				std::cout << cameraMatrixOutput << "\n\n";
				if (allCharucoIds.size() != 1) std::cout << "Difference:\n" << cameraMatrixOutput - prevCamera << "\n\n";
#else
				allCorners.insert(allCorners.cend(), markerCorners.cbegin(), markerCorners.cend());
				allIds.insert(allIds.cend(), markerIds.cbegin(), markerIds.cend());
				counter.push_back((int)markerCorners.size());
				double err = cv::aruco::calibrateCameraAruco(allCorners, allIds, counter, board, imgSize, cameraMatrixOutput, distCoeffsOutput);
				std::cout << cameraMatrix << "\n\n";
				if (counter.size() != 1) std::cout << "Difference:\n" << cameraMatrix - prevCamera << "\n\n";
#endif // CALIBRATE_CH
			}
			break;
		case 13:
			printf("done\n");
			break;
		default:
			break;
		}
		if (key == 13) {
			if (cameraMatrixOutput.total())break;
			else { printf("Calibration imcomplete. Try with at least 1 frame capture\n"); }
		}
		cv::aruco::drawDetectedMarkers(img, markerCorners, markerIds);
		cv::imshow("calibrating..", img);
	};
	cv::destroyWindow("calibrating..");
	system("cls");
	system("clear");
}

void detectNShowArUco(cv::VideoCapture& stream, const cv::Ptr<cv::aruco::Dictionary>& dict, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs) {
	std::vector<int> markerIds;
	std::vector<std::vector<cv::Point2f>> markerCorners;
	cv::Mat img;
	printf("Press Return key to escape\n");
	while (1) {		// aruco 감지/포즈 판별 및 화면상 출력
		int key;
		if (!stream.read(img)) {
			fprintf(stderr, "No video input device.\n");
			break;
		}
		cv::aruco::detectMarkers(img, dict, markerCorners, markerIds);
		cv::aruco::drawDetectedMarkers(img, markerCorners, markerIds);
		std::vector<cv::Vec3d> rvs, tvs;
		cv::aruco::estimatePoseSingleMarkers(markerCorners, CHARUCOGRID_SIDE, cameraMatrix, distCoeffs, rvs, tvs);
		
		for (size_t i = 0; i < rvs.size(); i++) {
			cv::drawFrameAxes(img, cameraMatrix, distCoeffs, rvs[i], tvs[i], CHARUCOGRID_SIDE, 2);
		}

		cv::imshow("ArUco Pose", img);
		key = cv::waitKey(17);
		if (key == 13) break;
	}
	cv::destroyWindow("ArUco Pose");
}

void detectNDrawArUco(cv::Mat& img, const cv::Ptr<cv::aruco::Dictionary>& dict, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs) {
	std::vector<int> markerIds;
	std::vector<std::vector<cv::Point2f>> markerCorners;
	cv::aruco::detectMarkers(img, dict, markerCorners, markerIds);
	cv::aruco::drawDetectedMarkers(img, markerCorners, markerIds);
	//cv::aruco::estimatePoseSingleMarkers(markerCorners,21.0f,cameraMatrix,distCoeffs,)
}
