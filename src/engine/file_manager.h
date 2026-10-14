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

const char* FILE_TYPE_STR[] = {
	"NONE",
	"IMAGE",
	"SCRIPT",
	"MATERIAL",
	"MODEL",
	"OTHER"
};


class File {
public:
	std::string name;
	AssetHandle* assetHandle = nullptr;

	int icon = -1; //opengl texture
	
	std::filesystem::file_time_type lastWriteTime{};

	char* data;
	uint64_t dataSize = 0;

	bool modified = false;

	FileType type = FileType::NONE;

};



class FileManager {
public:
	std::filesystem::path projectFilesPath;

	std::unordered_map<std::string, File*> projectFiles;

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



	void readProjectFiles() {
		logInfo("[FILE MANAGER] Read files from project directory");

		for (const std::filesystem::directory_entry& entry :
			std::filesystem::directory_iterator(projectFilesPath)) {

		
			if (entry.is_directory()) {
			}
			else {
				File* file = nullptr;

				if (auto it = projectFiles.find(entry.path().filename().string());
					it != projectFiles.end()) {

					std::filesystem::file_time_type lastWriteTime = entry.last_write_time();
					
					file = projectFiles.at(it->first);

					if (lastWriteTime != it->second->lastWriteTime ) {
						logInfo("[FILE MANAGER] file ", file->name," modified");

						
						file->modified = true;

					}
					
				}
				else {
					logInfo("[FILE MANAGER] First load: ", entry);
					file = new File();
					file->name = entry.path().filename().string();
					file->lastWriteTime = std::filesystem::last_write_time(entry.path());
					projectFiles[file->name] = file;
				}


				std::ifstream f(entry.path(), std::ios::binary);
				if (f.is_open()) {
					f.seekg(0, std::ios::end);
					std::streampos size = f.tellg();
					f.seekg(0, std::ios::beg);

					char* data = new char[size];
					f.read(data, size);

					file->data = data;
					file->dataSize = size;
				}
				else {
					logError("Failed to open");
				}
			}	
		}
	}

	void reload() {
		logInfo("[FILE MANAGER] Reload files");

		std::unordered_map<std::string, Material*> materialsNeedTexture;
		std::unordered_map<std::string, AssetHandle*> textures;

		
		for (auto pair : projectFiles) {
			std::filesystem::path entry = pair.second->name;
			std::string filename = entry.filename().string();
			std::string extension = entry.extension().string();


			

			

			


			File* file = projectFiles.at(filename);

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

		for (auto pair : textures) {
			if (materialsNeedTexture.contains(pair.first)) {
				materialsNeedTexture[pair.first]->textureHandle = *pair.second;
			}
		}
	}
};