#define STB_IMAGE_IMPLEMENTATION

#pragma comment(lib, "opengl32.lib")


#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <Windows.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "json.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"



#include "../engine/asset_repository.h"
#include "../engine/loaders/image_loader.h"
#include "../engine/loaders/model_loader.h"
#include "../engine/loaders/drish_loader.h"
#include "../engine/shader.h"
#include "../engine/transform.h"
#include "../engine/game_objects/node.h"
#include "../engine/game_objects/model.h"
#include "../engine/game_objects/camera.h"
#include "../engine/graphics.h"
#include "../engine/window.h"
#include "../engine/node_creator.h"
#include "../engine/lua_runner.h"
#include "../engine/logger.h"
#include "../version.h"
#include "../engine/game_config.h"
#include "../engine/node_repository.h"
#include "../engine/looper.h"
#include "../consts.h"
#include "../engine/file_manager.h"

const double targetFPS = 60.0;
const double frameDuration = 1.0 / targetFPS;

int main(int argc, char* argv[]) {
	logInfo("Hello");
	logInfo("Version", DRISH_ENGINE_VERSION_MAJOR,
		DRISH_ENGINE_VERSION_MINOR);

	std::filesystem::path drishPath = std::filesystem::path("world.drish");
	if (argc > 1) {
		drishPath = std::filesystem::path(argv[1]);
	}
	logInfo("Drish file path: ", drishPath.string().c_str());


	drishengine::Window window = drishengine::Window(500, 500);
	Graphics graphics = Graphics();
	graphics.init(&window);

	Looper looper;
	AssetRepository assetRepository;
	NodeRepository nodeRepository;
	Node* world = NodeCreator::createNode(Type::BASE, "World", &nodeRepository);
	FileManager fileManager = FileManager(&assetRepository);
	fileManager.projectFilesPath = drishPath.parent_path() / PROJECT_FILES_DIRECTORY_NAME;
	//fileManager.reload();

	GameConfig gameConfig;





	std::map<std::string, File*> files;


	std::ifstream data("data.bin", std::ifstream::binary);
	if (data) {
		char signature[5];

		data.read(signature, 5);

		if (std::string(signature) != "DRISH") {
			logError("Signature not found");
		}

		uint64_t drishJsonSize = 0;
		data.read((char*)(&drishJsonSize), sizeof(drishJsonSize));
		logInfo("Json Drish size: ", drishJsonSize);

		char* jsonFileData = new char[drishJsonSize];
		data.read(jsonFileData, drishJsonSize);

		

		uint64_t filesCount = 0;
		data.read((char*)(&filesCount), sizeof(filesCount));
		logInfo("Files count: ", filesCount);

		for (uint64_t i = 0; i < filesCount; i++) {
			uint64_t filenameLength = 0;
			data.read((char*)(&filenameLength), sizeof(filenameLength));
			logInfo("Filename size: ", filenameLength);

			char* filename = new char[filenameLength];
			data.read(filename, filenameLength);
			std::string filenameStr = std::string(filename, filenameLength);
			logInfo("Filename: ", filenameStr);
			


			uint64_t dataSize = 0;
			data.read((char*)(&dataSize), sizeof(dataSize));
			logInfo("Data size: ", dataSize);

			char* filedata = new char[dataSize];
			data.read(filedata, dataSize);

			File* file = new File();
			file->name = filenameStr;
			file->data = filedata;
			file->dataSize = dataSize;


			fileManager.projectFiles[file->name] = file;

			delete[] filename;
			//delete[] filedata;
		}

		fileManager.reload();

		DrishLoader::load(drishPath, world, &assetRepository,
			&gameConfig, &nodeRepository, &fileManager, jsonFileData, drishJsonSize
		);

		delete[] jsonFileData;

		

	}
	else {
		logError("[GAME] Loading failed - failed to open data.bin");
		MessageBox(NULL, "Failed to open data.bin", "Error", MB_ICONERROR | MB_OK);
		return 0;
	}


	

	
	


	logDebug("[GAME] Width: ", gameConfig.width);
	logDebug("[GAME] Height: ", gameConfig.height);
	logDebug("[GAME] Title: ", gameConfig.title);;
	logDebug("[GAME] Use Fullscreen: ", gameConfig.useFullscreen);;
	window.resize(gameConfig.width, gameConfig.height);
	window.renameTitle(gameConfig.title);
	if (gameConfig.useFullscreen) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(window.getWindow(), glfwGetPrimaryMonitor(), 0, 0, gameConfig.width,
			gameConfig.height, targetFPS);
	}

	
	LuaRunner luaRunner;
	luaRunner.setGetKey(&window);


	glViewport(0, 0, gameConfig.width, gameConfig.height);

	luaRunner.updateNodesScriptEnvironment(world, &assetRepository);


	double lastTime = glfwGetTime();

	while (!window.shouldClose()) {
		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;

		if (delta >= frameDuration) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			world->update(delta);
			looper.loop(&nodeRepository, &assetRepository);
			glBindVertexArray(0);
			glfwSwapBuffers(window.getWindow());

			lastTime = glfwGetTime();
		}

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}