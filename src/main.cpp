#include "module.h"

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

namespace onart {
	/// <summary>
	/// OpenGL을 이용한 렌더링 과정입니다.
	/// </summary>
	void cgLoop();
}

cv::VideoCapture webcamStream;
cv::Mat cameraMatrix, distCoeffs;
cv::Ptr<cv::aruco::Dictionary> dict;

int main(int argc, char* argv[]) {
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());
	webcamStream.open(0);

	cv::Mat img;

	dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

	calibrate(webcamStream, dict, cameraMatrix, distCoeffs);
	detectNShowArUco(webcamStream, dict, cameraMatrix, distCoeffs);

	// (추가목표) 그래픽스 카메라 구성, 프레임버퍼로 그림
	// (추가목표) aruco 위치와 포즈 기준으로 모델행렬 구성, 모델 렌더링
	onart::cgLoop();
}
