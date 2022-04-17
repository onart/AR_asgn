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

const float MARKER1_SIDE = 0.124f;
const float MARKERBOARD_SIDE = 0.021f;
const float MARKERGRID_GAP = 0.007f;
const float CHARUCOGRID_SIDE = 0.021f;
const float CHARUCOMARKER_SIDE = 0.011f;

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
				cv::aruco::estimatePoseSingleMarkers(markerCorners, MARKER1_SIDE, cameraMatrix, distCoeffs, rvs, tvs);
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
		worldprog["proj"] = mat4::perspective(PI / 6, 4.0f / 3.0f, 0, 3000.0f);
		print(mat4::perspective(PI / 6, 4.0f / 3.0f, 0, 3000.0f));
		glGenTextures(1, &cuboTex);
		glBindTexture(GL_TEXTURE_2D, cuboTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void render1() {
		cv::Mat img;
		webcamStream.read(img);	// 본인 프로세서 및 캠 기준, 여기만 평균 1/40~1/30초 가량 걸림. 여기서 오는 프레임 저하를 막으려면 당장에는 FFMPEG 저수준에 직접 접근하는 방법 말고는 떠오르지 않음
		//detectNDrawArUco(img, dict, cameraMatrix, distCoeffs);
		Quaternion rot;
		vec3 pos;
		bool d1 = detect1(img, rot, pos);
		dat2planar(img);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bgprog.use();
		glBindVertexArray(rect);
		glBindTexture(GL_TEXTURE_2D, bgTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		worldprog.use();
		glBindVertexArray(cubo);
		glBindTexture(GL_TEXTURE_2D,cuboTex);
		if (d1) {
			worldprog["model"] = mat4::translate(vec3(0,0,-5)) * (-rot).toMat4() * mat4::scale(0.2f);
		}
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
		glfwSwapBuffers(window);
	}
}