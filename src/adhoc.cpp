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
		constexpr float ln100 = 4.605170185988f;	// 실제 ln100보다 약 6.40e-8만큼 큼
		constexpr float delay = ln100 / 0.5f;		// 분모=마커 위치에 99%만큼 다가갈 때까지 걸리는 시간(초), 필수 조건: 프레임 타임보다 길 것
		// delay가 길수록: 카메라 보정에 의한 축 진동과 z 역산에 의한 떨림이 자연스럽게 보정
		// delay가 짧을수록: 회전 보간 한계값에 의한 역회전 방지
		static cv::Mat img;
		webcamStream.read(img);	// 본인 프로세서 및 캠 기준, 여기만 평균 1/40~1/30초 가량 걸림. 여기서 오는 프레임 저하를 막으려면 당장에는 FFMPEG 저수준에 직접 접근하는 방법 말고는 떠오르지 않음
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
			// 방법 1: 카메라 보정값을 믿고 회전과 위치를 적용. intrinsic 행렬만 사용하므로 카메라 중심이 (0,0,0), 카메라가 바라보는 방향이 +z방향. 렌더링과 방향을 맞춤(-desiredPos)
			// rotation(vec3(1, 1, 0), PI): 기본 방향 보정(bottom면이 마커와 마주봄), translate(-1): 기본 위치 보정(중심을 빨간 원 부분에서 마커 중심으로, 그리고 밑면과 마커를 이론상 맞닿게. 프로그램에서 정의한 단위정육면체는 모서리 길이가 2임)
			worldprog["model"] = mat4::TRS(currentPosition, currentRotation * Quaternion::rotation(vec3(1, 1, 0), PI), AUGMENT_UNIT * 0.5f) * mat4::translate(-1);
			
		}
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
		glfwSwapBuffers(window);
	}
}