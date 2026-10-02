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


	char* data;
	uint64_t dataSize = 0;

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


	//void loadScript(Script* script) {
	//	std::ifstream file(projectFilesPath / script->name);
	//	if (file.is_open()) {
	//		std::stringstream buf;
	//		buf << file.rdbuf();
	//		script->source = buf.str();
	//	}
	//	else {
	//		logError("Script import stream is closed");
	//	}
	//}

	void writeScript(Script* script) {
		std::ofstream f(projectFilesPath / script->name);
		if (f.is_open()) {
			f << script->source;
		}
	}


	void load() {

	}

	void reload() {
		std::map<std::string, Material*> materialsNeedTexture;
		std::map<std::string, AssetHandle*> textures;

		/*for (const std::filesystem::directory_entry& entry :
			std::filesystem::directory_iterator(projectFilesPath)) {*/
		for (auto pair : projectFiles) {
		



			//if (entry.is_directory()) {
			if (false) {
			}
			else {
				std::filesystem::path entry = pair.second->name;
				std::string filename = entry.filename().string();
				std::string extension = entry.extension().string();

				if (false) {

				}
				else {
					logInfo("First load: ", entry);

					File* file = new File();
					file->name = filename;
					projectFiles[filename] = file;

					if (extension == ".mat") {
						Material* material = new Material();
						material->name = filename;
						material->shader = &assetRepository->defaultShader;
						
						file->type = FileType::MATERIAL;


						nlohmann::json json = nlohmann::json::parse(pair.second->data, pair.second->data +
							pair.second->dataSize);

						material->color.r = json["color"]["r"];
						material->color.g = json["color"]["g"];
						material->color.b = json["color"]["b"];
						material->shine = json["shine"];
						material->useLight = json["useLight"];

						if (json.contains("texture")) {
							materialsNeedTexture[json["texture"]] = material;
						}

						file->assetHandle = assetRepository->materials.add(material);
	
								
					}

					if (extension == ".lua") {
						Script* script = new Script();
						script->name = filename;
						file->assetHandle = assetRepository->scripts.add(script);
						file->type = FileType::SCRIPT;

						script->source = std::string(pair.second->data, pair.second->dataSize);

						//loadScript(script);
						

					}
					if (extension == ".png" || extension == ".jpg" || extension == "jpeg") {
						Texture* texture = new Texture();
						texture->name = filename;

						

						ImageLoaderError err = ImageLoader::loadImage((unsigned char*)pair.second->data, pair.second->dataSize, texture);
						if (err != ImageLoaderError::OK) {
							delete texture;
							logError("Error loading: ", entry.string());
						}
						else {
							file->assetHandle = assetRepository->textures.add(texture);
							textures[texture->name] = file->assetHandle;
							
							file->type = FileType::IMAGE;
							file->icon = texture->glid;
						}

					}
					if (extension == ".obj") {
						Vertices* vertices = new Vertices();
						vertices->name = filename;

						drishengine::loadObj(pair.second->data, pair.second->dataSize, vertices);
						file->assetHandle = assetRepository->vertices.add(vertices);
						file->type = FileType::MODEL;
						logInfo("Generating buffers");
						vertices->createBuffers();

					}
				}



			}

		}

		for (auto pair : textures) {
			if (materialsNeedTexture.contains(pair.first)) {
				materialsNeedTexture[pair.first]->textureHandle = *pair.second;
			}
		}

	}
};