#pragma once


#include <optional>

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
	std::string name; //contains extension 
	std::string extension; 

	AssetHandle* assetHandle = nullptr;

	int icon = -1; //opengl texture
	
	std::filesystem::file_time_type lastWriteTime{};

	char* data;
	uint64_t dataSize = 0;

	bool imported = false;

	FileType type = FileType::NONE;

};



class FileManager {
public:
	FileManager(const std::filesystem::path& projectFilesPath) 
		: projectFilesPath{ projectFilesPath } 
	{};

	const std::unordered_map<std::string, File*>& getProjectFiles() const {
		return this->projectFiles;
	}

	void addFile(File* file) {
		auto it = this->projectFiles.find(file->name);
		if (it == this->projectFiles.end()) {
			projectFiles[file->name] = file;
			logInfo("[FILE MANAGER] file ", file->name, " added");
		}
		else {
			logWarning("[FILE MANAGER] file ", file->name, " already inside");
		}

		
	}

	std::optional<File*> getFile(std::string name) {
		auto it = this->projectFiles.find(name);

		if (it == this->projectFiles.end()) {
			return std::nullopt;
		}

		return it->second;
	}

	void loadArchive(std::filesystem::path path) {

	}

	void readProjectFiles() {
		logInfo("[FILE MANAGER] Read files from project directory");

		for (const std::filesystem::directory_entry& entry :
			std::filesystem::directory_iterator(this->projectFilesPath)) {

		
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

						
						file->imported = false;

					}
					
				}
				else {
					logInfo("[FILE MANAGER] First load: ", entry);
					file = new File();
					file->name = entry.path().filename().string();
					file->extension = entry.path().filename().extension().string();
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
private:
	std::unordered_map<std::string, File*> projectFiles;
	std::filesystem::path projectFilesPath;
};