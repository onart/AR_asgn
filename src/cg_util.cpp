#include "oagle.h"
#include "oaglem.h"

GLFWwindow* window;
int frame;

namespace onart {
	bool init() {
		window = createWindow("CG", 1280, 720);
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LEQUAL);
		glViewport(0, 0, 1280, 720);
		return true;
	}


	void update() {

	}

	void render() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwSwapBuffers(window);
	}

	void cgLoop() {
		if (!init()) return;
		for (frame = 1; !glfwWindowShouldClose(window); frame++) {
			glfwPollEvents();
			update();
			render();
		}
		destroyWindow(window);
	}
}