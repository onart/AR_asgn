#include "oagle.h"
#include "oaglem.h"
#include "Shader.h"

GLFWwindow* window;
GLuint rect, bgTex, cubo;
double tp = 0;
double dt = 0;
int frame;
void(*rd)(void);

onart::Shader bgprog, worldprog;

namespace onart {

	struct Vertex {
		vec3 pos;
		vec3 norm;
		vec2 tc;
	};

	/// <summary>
	/// id=1 마커 위에 정육면체를 렌더링
	/// </summary>
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
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void makeCubo() {
		static GLuint cuboVBO = 0, cuboIBO = 0;
		if (cuboVBO) {
			glDeleteBuffers(1, &cuboVBO);
			glDeleteBuffers(1, &cuboIBO);
			cuboVBO = cuboIBO = 0;
		}
		if (cubo) {
			glDeleteVertexArrays(1, &cubo);
			cubo = 0;
		}
		const static Vertex cube[] = {
			{vec3(-1,-1,-1),vec3(0,0,-1),vec2(1.0f / 2, 0)},
			{vec3(-1,-1,-1),vec3(0,-1,0),vec2(3.0f / 4, 1.0f / 3)},
			{vec3(-1,-1,-1),vec3(-1,0,0),vec2(3.0f / 4, 1.0f / 3)},
				
			{vec3(-1,-1,+1),vec3(0,0,+1),vec2(1.0f / 2, 1)},
			{vec3(-1,-1,+1),vec3(0,-1,0),vec2(3.0f / 4, 2.0f / 3)},
			{vec3(-1,-1,+1),vec3(-1,0,0),vec2(3.0f / 4, 2.0f / 3)},
	
			{vec3(-1,+1,-1),vec3(0,0,-1),vec2(1.0f / 4, 0)},
			{vec3(-1,+1,-1),vec3(0,1,0), vec2(0, 1.0f / 3)},
			{vec3(-1,+1,-1),vec3(-1,0,0),vec2(1, 1.0f / 3)},

			{vec3(-1,+1,+1),vec3(0,0,1),vec2(1.0f / 4, 1)},
			{vec3(-1,+1,+1),vec3(0,1,0),vec2(0, 2.0f / 3)},
			{vec3(-1,+1,+1),vec3(-1,0,0),vec2(1, 2.0f / 3)},
	
			{vec3(+1,-1,-1),vec3(1,0,0),vec2(1.0f / 2, 1.0f / 3)},
			{vec3(+1,-1,-1),vec3(0,-1,0),vec2(1.0f / 2, 1.0f / 3)},
			{vec3(+1,-1,-1),vec3(0,0,-1),vec2(1.0f / 2, 1.0f / 3)},
	
			{vec3(+1,-1,+1),vec3(1,0,0),vec2(1.0f / 2, 2.0f / 3)},
			{vec3(+1,-1,+1),vec3(0,-1,0),vec2(1.0f / 2, 2.0f / 3)},
			{vec3(+1,-1,+1),vec3(0,0,1),vec2(1.0f / 2, 2.0f / 3)},
	
			{vec3(+1,+1,-1),vec3(1,0,0),vec2(1.0f / 4, 1.0f / 3)},
			{vec3(+1,+1,-1),vec3(0,1,0),vec2(1.0f / 4, 1.0f / 3)},
			{vec3(+1,+1,-1),vec3(0,0,-1),vec2(1.0f / 4, 1.0f / 3)},

			{vec3(+1,+1,+1),vec3(1,0,0),vec2(1.0f / 4, 2.0f / 3)},
			{vec3(+1,+1,+1),vec3(0,1,0),vec2(1.0f / 4, 2.0f / 3)},
			{vec3(+1,+1,+1),vec3(0,0,1),vec2(1.0f / 4, 2.0f / 3)},
		};
		const static unsigned indices[] = {
			0,6,20,0,20,14,
			18,21,15,18,15,12,
			23,3,17,9,3,23,
			7,10,22,7,22,19,
			2,11,8,2,5,11,
			1,16,4,1,13,16
		};
		glGenBuffers(1, &cuboVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cuboVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
		glGenBuffers(1, &cuboIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cuboIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glCreateVertexArrays(1, &cubo);
		glBindVertexArray(cubo);
		glBindBuffer(GL_ARRAY_BUFFER, cuboVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cuboIBO);
		glEnableVertexAttribArray(0);	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
		glEnableVertexAttribArray(1);	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
		glEnableVertexAttribArray(2);	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tc));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
	
	void getesc(GLFWwindow* window, int key, int scanCode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE) glfwDestroyWindow(window);
	}
	bool init() {
		window = createWindow("CG", 640, 480);
		if (!window || !onart::initExtensions(window)) {
			return false;
		}
		glfwSetKeyCallback(window, getesc);
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LEQUAL);
		glViewport(0, 0, 640, 480);
		glActiveTexture(GL_TEXTURE0);	// adhoc에서는 텍스처를 1종류만 쓸 것
		bgprog.initWithFile("bg.vert", "bg.frag");
		worldprog.initWithFile("world.vert", "world.frag");
		bgprog["image"] = 0;
		makeRect();
		makeCubo();
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
			static double prev = 0;
			tp = glfwGetTime();
			//printf("frame time=1/%f\r", 1 / (t - tp));
			dt = tp - prev;
			prev = tp;
		}
		destroyWindow(window);
	}
}