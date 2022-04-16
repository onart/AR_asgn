#include "oagle.h"
#include "oaglem.h"
#include "Shader.h"

GLFWwindow* window;
GLuint rect, bgTex;
double tp = 0;
int frame;
void(*rd)(void);

onart::Shader bgprog;

namespace onart {
	
	void render1();
	void init1();

	void render() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwSwapBuffers(window);
	}

	void makeRect() {
		static GLuint rectVBO = 0;
		constexpr float vtx[] = {
			-1.0f,-1.0f,0.0f,0.0f,	// LD
			1.0f,-1.0f,1.0f,0.0f,	// RD
			1.0f,1.0f,1.0f,1.0f,	// RU
			1.0f,1.0f,1.0f,1.0f,	// RU
			-1.0f,1.0f,0.0f,1.0f,	// LU
			-1.0f,-1.0f,0.0f,0.0f	// LD
		};
		if (rectVBO) {
			glDeleteBuffers(1, &rectVBO);
			rectVBO = 0;
		}
		if (rect) {
			glDeleteVertexArrays(1, &rect);
			rect = 0;
		}
		glGenBuffers(1, &rectVBO);
		glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vtx), vtx, GL_STATIC_DRAW);
		glGenVertexArrays(1, &rect);
		glBindVertexArray(rect);
		glEnableVertexAttribArray(0);	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
		glEnableVertexAttribArray(1);	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
		glBindVertexArray(0);
	}

	GLuint makeTextureObject() {
		GLuint tx;
		glGenTextures(1, &tx);
		glBindTexture(GL_TEXTURE_2D, tx);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		return tx;
	}

	bool init() {
		window = createWindow("CG", 1280, 720);
		if (!window || !onart::initExtensions(window)) {
			return false;
		}
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LEQUAL);
		glViewport(0, 0, 1280, 720);
		glActiveTexture(GL_TEXTURE0);	// adhoc에서는 텍스처를 1종류만 쓸 것
		bgprog.initWithFile("bg.vert", "bg.frag");
		bgprog["image"] = 0;
		makeRect();
		bgTex = makeTextureObject();
		rd = render1;
		init1();
		return true;
	}

	void cgLoop() {
		if (!init()) return;
		for (frame = 1; !glfwWindowShouldClose(window); frame++) {
			glfwPollEvents();
			rd();
			double t = glfwGetTime();
			//printf("frame time=1/%f\r", 1 / (t - tp));
			tp = t;
		}
		destroyWindow(window);
	}
}