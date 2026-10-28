#pragma once

#include <string>

#include "logger.h"
#include "gl_common.h"

namespace drishengine {

	inline void error_callback(int error, const char* description);
	inline void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	

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


		void resizeCallback(int width, int height) {
			this->height = height;
			this->width = width;
			this->windowAspectRatio = (float) width / height;
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


			glfwSetWindowUserPointer(window, this);

			if (window == nullptr) {
				logError("[WINDOW]", "Can't create window");
				return -1;
			}
			glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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


	void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);

		Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

		win->resizeCallback(width, height);
	}
}

//void error_callback(int error, const char* description) {
//	fprintf(stderr, description);
//	system("pause");
//}
//
