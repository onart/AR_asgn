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

static cv::Mat camImage;

void detectNDrawArUco(cv::Mat& img, const cv::Ptr<cv::aruco::Dictionary>& dict, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs);

namespace onart {

	inline mat4 to_oam(const cv::Mat& mat) {
		return mat4(
			mat.at<double>(0, 0), mat.at<double>(0, 1), mat.at<double>(0, 2), 0,
			mat.at<double>(1, 0), mat.at<double>(1, 1), mat.at<double>(1, 2), 0,
			mat.at<double>(2, 0), mat.at<double>(2, 1), mat.at<double>(2, 2), 0,
			0, 0, 0, 1
		);
	}

	inline vec3 to_oav(const cv::Vec3d& position) {
		return vec3(position[0], position[1], position[2]);
	}

	inline Quaternion to_oaq(const cv::Vec3d& rotation) {
		return Quaternion::rotation(to_oav(rotation), cv::norm(rotation));
	}

	/// <summary>
	/// 1번 마커를 찾으면 위치를 그리고 카메라 행렬에 따라 보정한 포즈를 리턴
	/// </summary>
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

	/// <summary>
	/// 1번 마커를 찾아 화면상 모퉁이 좌표를 리턴
	/// </summary>
	bool detect2(cv::Mat& img, std::vector<cv::Point2f>& corners) {
		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners;

		cv::aruco::detectMarkers(img, dict, markerCorners, markerIds);
		for (int i = 0; i < markerIds.size(); i++) {
			if (markerIds[i] == 1) {
				std::vector<std::vector<cv::Point2f>> corner1{ markerCorners[i] };
				cv::aruco::drawDetectedMarkers(img, corner1);
				for (auto& pt : markerCorners[i]) {
					//std::cout << pt << '\n';
					pt.x /= 320;
					pt.x -= 1;
					pt.y /= 240;
					pt.y -= 1;
				}
				//std::cout << "\n\n";
				corners.swap(markerCorners[i]);
				return true;
			}
		}
		return false;
	}

	/// <summary>
	/// bgTex의 데이터에 해당 이미지를 대입
	/// </summary>
	void dat2planar(const cv::Mat& img) {
		glBindTexture(GL_TEXTURE_2D, bgTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void init1() {
		bgprog["mode"] = 1;
		bgprog["homography"] = mat4();
		worldprog["view"] = mat4();
		system("cls"); printf("move marker 1 to move this cube\n");
		
		static bool nonce = false;
		if (nonce) return;
		nonce = true;
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
		webcamStream.read(camImage);	// 본인 프로세서 및 캠 기준, 여기만 평균 1/40~1/30초 가량 걸림. 여기서 오는 프레임 저하를 막으려면 당장에는 FFMPEG 저수준에 직접 접근하는 방법 말고는 떠오르지 않음
		static Quaternion desiredRot;
		static vec3 desiredPos;
		bool d1 = detect1(camImage, desiredRot, desiredPos);
		dat2planar(camImage);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bgprog.use();
		glBindVertexArray(rect);
		glBindTexture(GL_TEXTURE_2D, bgTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		worldprog.use();
		glBindVertexArray(cubo);
		glBindTexture(GL_TEXTURE_2D,cuboTex);
		if (d1) {
			// 방법 1: 카메라 보정값을 믿고 회전과 위치를 적용. intrinsic 행렬만 사용하므로 카메라 중심이 (0,0,0), 카메라가 바라보는 방향이 +z방향. 렌더링과 방향을 맞춤(-desiredPos)
			//currentPosition = lerp(currentPosition, -desiredPos, dt * delay);
			desiredPos += desiredRot.toMat3() * vec3(1, 1, 0) * AUGMENT_UNIT * 0.5f;	// -> 좌표를 마커 중심의 것으로
			currentPosition = -desiredPos;
			print(currentPosition);
			//print(desiredPos, "pos", ' '); print(desiredRot, "rot", '\r');	// 정확히 카메라를 보고 있을 때 면 법선은 (0,0,1), x축은 카메라 입장에서 오른쪽 방향, y축은 카메라 입장에서 아래 방향
			currentRotation = lerp(currentRotation, desiredRot, dt * delay);
			// rotation(vec3(1, 1, 0), PI): 기본 방향 보정(bottom면이 마커와 마주봄), scale: 마커와 크기를 최대한 비슷하게
			worldprog["model"] = mat4::TRS(currentPosition, currentRotation * Quaternion::rotation(vec3(1, 1, 0), PI), AUGMENT_UNIT * 0.5f);
		}
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
		glfwSwapBuffers(window);
	}

	void adhocFinalize() {
		glDeleteTextures(1, &bgTex);
		glDeleteTextures(1, &cuboTex);
		webcamStream.release();
	}

	void init2() {
		system("cls"); printf("An image will be attached to marker 1\n");
		bgprog["mode"] = 2;
	}

	void render2() {
		std::vector<cv::Point2f> corners;
		std::vector<cv::Point2f> base{
			cv::Point2f(-1,1), cv::Point2f(-1,-1), cv::Point2f(1,-1), cv::Point2f(1,1)	// (반시계 방향 맞음)
		};
		webcamStream.read(camImage);
		bool d1 = detect2(camImage, corners);
		dat2planar(camImage);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bgprog.use();
		glBindVertexArray(rect);
		bgprog["homography"] = mat4();
		glBindTexture(GL_TEXTURE_2D, bgTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		if (d1) {
			cv::Mat hom = cv::findHomography(base, corners);
			
			bgprog["homography"] = to_oam(hom);
			glBindTexture(GL_TEXTURE_2D, cuboTex);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glfwSwapBuffers(window);
	}

	void init3() {
		system("cls"); printf("Marker 1 is now at (0,0,0), and the world coordinate unit will be meters\n");
		bgprog["mode"] = 1;
		bgprog["homography"] = mat4();
	}

	void render3() {
		webcamStream.read(camImage);
		dat2planar(camImage);
		static Quaternion desiredRot, currentRotation;
		static vec3 desiredPos;
		static int count = 0;
		
		bool d1 = detect1(camImage, desiredRot, desiredPos);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bgprog.use();
		glBindVertexArray(rect);
		glBindTexture(GL_TEXTURE_2D, bgTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		if (d1) {
			constexpr float ln100 = 4.605170185988f;	// 실제 ln100보다 약 6.40e-8만큼 큼
			constexpr float delay = ln100 / 2.0f;		// 분모=마커 위치에 99%만큼 다가갈 때까지 걸리는 시간(초), 필수 조건: 프레임 타임보다 길 것
			worldprog.use();
			currentRotation = lerp(currentRotation, desiredRot, dt * delay);
			vec3 dir = -currentRotation.toMat3().row(3);	// 원래 방향: (0,0,-1). 마커 직교 기저를 역으로 적용하여 이것을 표현: currentRotation.conjugate().toMat3()*vec3(0,0,-1)과 동일함
			worldprog["view"] = mat4::lookAt(desiredPos, desiredPos + dir, vec3(0, 1, 0));
			worldprog["model"] = mat4::scale(AUGMENT_UNIT * 0.2f);
			glBindVertexArray(cubo);
			glBindTexture(GL_TEXTURE_2D, cuboTex);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
			worldprog["model"] = mat4::TRS(vec3(0, 0.5f, 0), Quaternion(), AUGMENT_UNIT * 0.2f);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
			worldprog["model"] = mat4::TRS(vec3(0, -0.5f, 0), Quaternion(), AUGMENT_UNIT * 0.2f);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
			worldprog["model"] = mat4::TRS(vec3(0.5f, 0, 0), Quaternion(), AUGMENT_UNIT * 0.2f);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
			worldprog["model"] = mat4::TRS(vec3(-0.5f, 0, 0), Quaternion(), AUGMENT_UNIT * 0.2f);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
			worldprog["model"] = mat4::TRS(vec3(0, 0, 0.5f), Quaternion(), AUGMENT_UNIT * 0.2f);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
			worldprog["model"] = mat4::TRS(vec3(0, 0, -0.5f), Quaternion(), AUGMENT_UNIT * 0.2f);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
		}
		glfwSwapBuffers(window);
	}

}