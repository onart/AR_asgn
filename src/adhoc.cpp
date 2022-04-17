#include "externals/gl/glad/glad.h"
#include "oaglem.h"
#include "module.h"
#include "Shader.h"

extern GLFWwindow* window;
extern GLuint rect, bgTex, cubo;
extern onart::Shader bgprog, worldprog;

extern cv::VideoCapture webcamStream;
extern cv::Mat cameraMatrix, distCoeffs;
extern cv::Ptr<cv::aruco::Dictionary> dict;

extern double dt;

const float MARKER1_SIDE = 0.124f;
const float MARKERBOARD_SIDE = 0.021f;
const float MARKERGRID_GAP = 0.007f;
const float CHARUCOGRID_SIDE = 0.021f;
const float CHARUCOMARKER_SIDE = 0.011f;

const float AUGMENT_UNIT = MARKER1_SIDE;

GLuint cuboTex;

void detectNDrawArUco(cv::Mat& img, const cv::Ptr<cv::aruco::Dictionary>& dict, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs);

namespace onart {

	inline mat4 to_oam(const cv::Mat& mat) {
		return mat4(
			mat.at<double>(0, 0), mat.at<double>(0,1), mat.at<double>(0, 2),0,
			mat.at<double>(1, 0), mat.at<double>(1, 1), mat.at<double>(1, 2), 0,
			mat.at<double>(2, 0), mat.at<double>(2, 1), mat.at<double>(2, 2), 0,
			0,0,0,1
			);
	}

	inline vec3 to_oav(const cv::Vec3d& position) {
		return vec3(position[0], position[1], position[2]);
	}

	inline Quaternion to_oaq(const cv::Vec3d& rotation) {
		return Quaternion::rotation(to_oav(rotation), cv::norm(rotation));
	}

	bool detect1(cv::Mat& img, Quaternion& rotation, vec3& position) {
		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners;
		
		cv::aruco::detectMarkers(img, dict, markerCorners, markerIds);
		for (int i = 0; i < markerIds.size(); i++) {
			if (markerIds[i] == 1) {
				std::vector<std::vector<cv::Point2f>> corner1{ markerCorners[i] };
				std::vector<cv::Vec3d> rvs, tvs;
				cv::aruco::estimatePoseSingleMarkers(markerCorners, AUGMENT_UNIT, cameraMatrix, distCoeffs, rvs, tvs);
				cv::aruco::drawDetectedMarkers(img, corner1);
				rotation = to_oaq(rvs[0]);
				position = to_oav(tvs[0]);
				return true;
			}
		}
		return false;
	}

	void dat2planar(const cv::Mat& img) {
		glBindTexture(GL_TEXTURE_2D, bgTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}



	void init1() {
		cv::Mat img = cv::imread("cube.png");
		worldprog["proj"] = mat4::perspective(PI / 6, 4.0f / 3.0f, 0, 1000.0f);
		glGenTextures(1, &cuboTex);
		glBindTexture(GL_TEXTURE_2D, cuboTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void render1() {
		static vec3 currentPosition;
		static Quaternion currentRotation;
		constexpr float ln100 = 4.605170185988f;	// ���� ln100���� �� 6.40e-8��ŭ ŭ
		constexpr float delay = ln100 / 0.5f;		// �и�=��Ŀ ��ġ�� 99%��ŭ �ٰ��� ������ �ɸ��� �ð�(��), �ʼ� ����: ������ Ÿ�Ӻ��� �� ��
		// delay�� �����: ī�޶� ������ ���� �� ������ z ���꿡 ���� ������ �ڿ������� ����
		// delay�� ª������: ȸ�� ���� �Ѱ谪�� ���� ��ȸ�� ����
		static cv::Mat img;
		webcamStream.read(img);	// ���� ���μ��� �� ķ ����, ���⸸ ��� 1/40~1/30�� ���� �ɸ�. ���⼭ ���� ������ ���ϸ� �������� ���忡�� FFMPEG �����ؿ� ���� �����ϴ� ��� ����� �������� ����
		static Quaternion desiredRot;
		static vec3 desiredPos;
		bool d1 = detect1(img, desiredRot, desiredPos);
		dat2planar(img);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bgprog.use();
		glBindVertexArray(rect);
		glBindTexture(GL_TEXTURE_2D, bgTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		worldprog.use();
		glBindVertexArray(cubo);
		glBindTexture(GL_TEXTURE_2D,cuboTex);
		currentPosition = lerp(currentPosition, -desiredPos, dt * delay);
		//print(desiredPos, "pos", ' '); print(desiredRot, "rot", '\r');
		currentRotation = slerp(currentRotation, desiredRot, dt * delay);
		if (d1) {
			// ��� 1: ī�޶� �������� �ϰ� ȸ���� ��ġ�� ����. intrinsic ��ĸ� ����ϹǷ� ī�޶� �߽��� (0,0,0), ī�޶� �ٶ󺸴� ������ +z����. �������� ������ ����(-desiredPos)
			// rotation(vec3(1, 1, 0), PI): �⺻ ���� ����(bottom���� ��Ŀ�� ���ֺ�), translate(-1): �⺻ ��ġ ����(�߽��� ���� �� �κп��� ��Ŀ �߽�����, �׸��� �ظ�� ��Ŀ�� �̷л� �´��. ���α׷����� ������ ����������ü�� �𼭸� ���̰� 2��)
			worldprog["model"] = mat4::TRS(currentPosition, currentRotation * Quaternion::rotation(vec3(1, 1, 0), PI), AUGMENT_UNIT * 0.5f) * mat4::translate(-1);
			
		}
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
		glfwSwapBuffers(window);
	}
}