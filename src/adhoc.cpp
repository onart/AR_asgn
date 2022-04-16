#include "externals/gl/glad/glad.h"
#include "oaglem.h"
#include "module.h"
#include "Shader.h"

extern GLFWwindow* window;
extern GLuint rect, bgTex;
extern onart::Shader bgprog;

cv::VideoCapture webcamStream;

namespace onart {

	void dat2planar(const cv::Mat& img) {
		glBindTexture(GL_TEXTURE_2D, bgTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void init1() {
		webcamStream.open(0);
	}

	void render1() {
		cv::Mat img;
		webcamStream.read(img);	// 본인 프로세서 및 캠 기준, 여기만 평균 1/40~1/30초 가량 걸림. 여기서 오는 프레임 저하를 막으려면 당장에는 FFMPEG 저수준에 직접 접근하는 방법 말고는 떠오르지 않음
		dat2planar(img);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bgprog.use();
		glBindVertexArray(rect);
		glBindTexture(GL_TEXTURE_2D, bgTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(window);
	}
}