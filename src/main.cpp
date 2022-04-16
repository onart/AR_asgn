#include "module.h"

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

namespace onart {
	/// <summary>
	/// OpenGL�� �̿��� ������ �����Դϴ�.
	/// </summary>
	void cgLoop();
}

int main(int argc, char* argv[]) {
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());
	cv::VideoCapture webcamStream(0);

	cv::Mat img;
	cv::Mat cameraMatrix, distCoeffs;

	cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

	calibrate(webcamStream, dict, cameraMatrix, distCoeffs);
	detectNShowArUco(webcamStream, dict, cameraMatrix, distCoeffs);
	webcamStream.release();

	// (�߰���ǥ) �׷��Ƚ� ī�޶� ����, �����ӹ��۷� �׸�
	// (�߰���ǥ) aruco ��ġ�� ���� �������� ����� ����, �� ������
	onart::cgLoop();
}
