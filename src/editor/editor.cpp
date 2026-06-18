#define STB_IMAGE_IMPLEMENTATION
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "json.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "imgui_docking/imgui.h"
#include "imgui_docking/imgui_impl_glfw.h"
#include "imgui_docking/imgui_impl_opengl3.h"
#include "imgui_docking/imgui_stdlib.h"
#include "imgui_docking/imgui_internal.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "editor.h"
#include "../engine/looper.h"
#include "../engine/node_repository.h"

const double targetFPS = 60.0;
const double frameDuration = 1.0 / targetFPS;

int main()
{
	/*ma_result result;
	ma_engine engine;

	result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS) {
		return -1;
	}

	ma_sound sound;

	result = ma_sound_init_from_file(&engine, "sound.wav", 0, NULL, NULL, &sound);
	if (result != MA_SUCCESS) {
		std::cout << ma_result_description(result);
		return -1;
	}



	ma_sound_set_looping(&sound, true);
	ma_sound_start(&sound)*/
	;
	// ma_sound_init_from_data_source()

	logInfo("Hello");
	logInfo("Version: ", DRISH_ENGINE_VERSION_BIG, ".",
		DRISH_ENGINE_VERSION_NOTBIGNOTSMALL, ".",
		DRISH_ENGINE_VERSION_SMALL, "+", DRISH_ENGINE_VERSION_ST);

	drishengine::Window window = drishengine::Window(800, 600);
	Graphics graphics = Graphics();
	graphics.init(&window);

	AssetRepository assetRepository;
	NodeRepository nodeRepository;
	Node *world = NodeCreator::createNode(Type::BASE, "World", &nodeRepository);

	LuaRunner scripter;
	scripter.addCreateNodesFunctions(&nodeRepository);
	scripter.addAssetRepositoryFunctions(&assetRepository);
	scripter.setGetKey(&window);


	Camera camera = Camera();
	camera.create_view = false;
	Editor editor(&window, &assetRepository, &nodeRepository, &scripter, &camera);
	editor.world = world;
	editor.graphics = &graphics;

	double lastTime = glfwGetTime();

	Shader lineShader;

	Looper looper;
	looper.currentCamera = &camera;

	while (!window.shouldClose())
	{
		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;

		if (delta >= frameDuration)
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			lineShader.use();
			lineShader.setMat4("projection", editor.camera->perspective);
			lineShader.setMat4("view", editor.camera->view);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			editor.loop(delta);

			editor.canvas->use();
			world->update(delta);

			looper.currentCamera = editor.camera;
			looper.loop(&nodeRepository, &assetRepository);
			world->drawInEditor(&lineShader);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glBindVertexArray(0);
			glfwSwapBuffers(window.getWindow());

			lastTime = glfwGetTime();
		}
		glfwPollEvents();
	}

	glfwTerminate();
	logInfo("Bye");
	return 0;
}
