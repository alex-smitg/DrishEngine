#pragma once

#include "gl_common.h"
#include "window.h"




class Graphics {
public:
	int init(drishengine::Window* window) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window->createWindow();
		/*if (!window->createWindow()) 
		{
			glfwTerminate();
			return -1;
		}*/

		glfwMakeContextCurrent(window->getWindow());
		//glfwSetErrorCallback(error_callback);
		//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

		if (glewInit() != GLEW_OK) 
		{
			logError("glewInit() not GLEW_OK");
			return -1;
		}

		this->enableMSAA();
		this->enableDepthTest();
		//this->enableVSYNC();

		

		return 0;
	}


	//void begin() {	
	//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	
	//}

	//void beginViewport() {
	//	/*glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);


	//	glViewport(0, 0, width, height);
	//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/
	//}

	//void endViewport() {
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	
	//}


	//void end(drishengine::Window* window) {
	//	glBindVertexArray(0);

	//	
	//	glfwSwapBuffers(window->getWindow());
	//	glfwPollEvents();
	//}

	void enableMSAA() {
		glfwWindowHint(GLFW_SAMPLES, 4);
	}

	void enableDepthTest() {
		glEnable(GL_DEPTH_TEST);
	}

	void enableVSYNC() {
		glfwSwapInterval(1);
	}

	~Graphics() {
		glfwTerminate();
	}
};