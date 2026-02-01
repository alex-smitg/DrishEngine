#pragma once

#include <string>

#include "logger.h"
#include "gl_common.h"

namespace drishengine {

	void error_callback(int error, const char* description);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	class Window {
	public:
		int width = 800;
		int height = 600;
		float windowAspectRatio = width / height;
		bool useFullscreen = false;
		std::string title = "Drish Engine";

		GLFWwindow* window = nullptr;

		Window(int width = 500, int height = 500, std::string title = "Drish Engine", bool useFullscreen = false) {
			this->height = height;
			this->width = width;
			this->title = title;
			this->useFullscreen = useFullscreen;
		}

		bool shouldClose() {
			return glfwWindowShouldClose(this->window);
		}

		int createWindow() {
			if (useFullscreen) {
				window = glfwCreateWindow(width, height, title.c_str(), glfwGetPrimaryMonitor(), nullptr);
			}
			else {
				window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);	
			}

			if (window == nullptr) {
				logError("[WINDOW]", "Can't create window");
				return -1;
			}
			return 0;
		}

		GLFWwindow* getWindow() {
			return window;
		}

		void resize(int width, int height) {
			glfwSetWindowSize(window, width, height);
		}

		void renameTitle(std::string title) {
			this->title = title;
			glfwSetWindowTitle(window, title.c_str());
		}
	};
}

//void error_callback(int error, const char* description) {
//	fprintf(stderr, description);
//	system("pause");
//}
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//	glViewport(0, 0, width, height);
//	windowWidth = width;
//	windowHeight = height;
//	float aspect = 1.0f;
//	
//	if (windowHeight != 0) {
//		aspect = (float)windowWidth / (float)windowHeight;
//	}
//	camera.setWindowAspectRatio(aspect);
//	
//}