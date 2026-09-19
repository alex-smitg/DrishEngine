#pragma once


#include "loaders/image_loader.h"
#include "loaders/model_loader.h"

#include "asset_repository.h"

#include "logger.h"


enum class FileType {
	NONE,
	IMAGE,
	SCRIPT,
	MATERIAL,
	MODEL,
	OTHER
};


class File {
public:
	std::string name = "";
	AssetHandle* assetHandle = nullptr;

	unsigned int icon = -1; //opengl texture

	FileType type = FileType::NONE;

};



class FileManager {
public:
	std::filesystem::path projectFilesPath;

	std::map<std::string, File*> projectFiles;

	AssetRepository * assetRepository = nullptr;

	FileManager(AssetRepository* assetRepository) {
		this->assetRepository = assetRepository;

	}


	void loadScript(Script* script) {
		std::ifstream file(projectFilesPath / script->name);
		if (file.is_open()) {
			std::stringstream buf;
			buf << file.rdbuf();
			script->source = buf.str();
		}
		else {
			logError("Script import stream is closed");
		}
	}

	void writeScript(Script* script) {
		std::ofstream f(projectFilesPath / script->name);
		if (f.is_open()) {
			f << script->source;
		}
	}

	void reload() {
		for (const std::filesystem::directory_entry& entry :
			std::filesystem::directory_iterator(projectFilesPath)) {



			if (entry.is_directory()) {
			}
			else {
				std::string filename = entry.path().filename().string();
				std::string extension = entry.path().extension().string();

				if (projectFiles.contains(filename)) {

				}
				else {
					logInfo("First load: ", entry.path());

					File* file = new File();
					file->name = filename;
					projectFiles[filename] = file;

					if (extension == ".mat") {
						Material* material = new Material();
						material->name = filename;
						material->shader = &assetRepository->defaultShader;
						
						file->type = FileType::MATERIAL;



						std::ifstream f(projectFilesPath / filename);
						if (f.is_open()) {
							nlohmann::json json = nlohmann::json::parse(f);
							material->color.r = json["color"]["r"];
							material->color.g = json["color"]["g"];
							material->color.b = json["color"]["b"];
							material->shine = json["shine"];
							material->useLight = json["useLight"];

							file->assetHandle = assetRepository->materials.add(material);
						}
						else {
							delete material;
							logError("Material import stream is closed");
						}
								
					}

					if (extension == ".lua") {
						Script* script = new Script();
						script->name = filename;
						file->assetHandle = assetRepository->scripts.add(script);
						file->type = FileType::SCRIPT;

						loadScript(script);
						

					}
					if (extension == ".png" || extension == ".jpg" || extension == "jpeg") {
						Texture* texture = new Texture();
						texture->name = filename;

						ImageLoaderError err = ImageLoader::loadImage(entry.path(), texture);
						if (err != ImageLoaderError::OK) {
							delete texture;
							logError("Error loading: ", entry.path().string());
						}
						else {
							file->assetHandle = assetRepository->textures.add(texture);
							file->type = FileType::IMAGE;
							file->icon = texture->glid;
						}

					}
					if (extension == ".obj") {
						Vertices* vertices = new Vertices();
						vertices->name = filename;

						drishengine::loadObj(projectFilesPath / filename, vertices);
						file->assetHandle = assetRepository->vertices.add(vertices);
						file->type = FileType::MODEL;
						logInfo("Generating buffers");
						vertices->createBuffers();

					}
				}



			}

		}
	}
};