#pragma once

#include <optional>

#include "loaders/image_loader.h"
#include "loaders/model_loader.h"

#include "asset_repository.h"

#include "logger.h"

enum class FileType
{
	NONE,
	IMAGE,
	SCRIPT,
	MATERIAL,
	MODEL,
	SOUND,
	OTHER
};

const char *FILE_TYPE_STR[] = {
	"NONE",
	"IMAGE",
	"SCRIPT",
	"MATERIAL",
	"MODEL",
	"SOUND",
	"OTHER"};

class File
{
public:
	std::string name; // contains extension
	std::string extension;

	AssetHandle *assetHandle = nullptr;

	int icon = -1; // opengl texture

	std::filesystem::file_time_type lastWriteTime{};

	std::vector<char> data;

	bool imported = false;

	FileType type = FileType::NONE;
};

class FileManager
{
public:
	FileManager(const std::filesystem::path &projectFilesPath)
		: projectFilesPath{projectFilesPath} {};

	const std::unordered_map<std::string, File *> &getProjectFiles() const
	{
		return this->projectFiles;
	}

	void addFile(File *file)
	{
		auto it = this->projectFiles.find(file->name);
		if (it == this->projectFiles.end())
		{
			projectFiles[file->name] = file;
			logInfo(SOURCE_LOCATION, "file", file->name, "added");
		}
		else
		{
			logWarning(SOURCE_LOCATION, "file", file->name, "already inside");
		}
	}

	template <typename T>
	void writeFile(const T& data, std::string name) {
		logInfo(SOURCE_LOCATION, "write file", name);
		std::ofstream f(this->projectFilesPath / name);
		if (f.is_open()) {
			f << data << std::endl;
			logInfo(SOURCE_LOCATION, "write ok");
		}
		else {
			logError(SOURCE_LOCATION, "stream is closed, file:", name);
		}
	}

	std::optional<File *> getFile(std::string name)
	{
		auto it = this->projectFiles.find(name);

		if (it == this->projectFiles.end())
		{
			return std::nullopt;
		}

		return it->second;
	}

	void writeArchive(std::ofstream& archive) {
		logInfo(SOURCE_LOCATION, "Write files");
		uint64_t filesCount = 0;

		for (const std::filesystem::directory_entry& entry :
			std::filesystem::directory_iterator(this->projectFilesPath)) {

			if (entry.is_directory()) {
			}
			else {
				filesCount++;
			}
		}

		archive.write((char*)&filesCount, sizeof(uint64_t));

		for (const std::filesystem::directory_entry& entry :
			std::filesystem::directory_iterator(this->projectFilesPath)) {




			if (entry.is_directory()) {
			}
			else {
				std::string filename = entry.path().filename().string();

				std::ifstream file(this->projectFilesPath / filename, std::ios::binary);
				if (file.is_open()) {
					std::stringstream buf;
					buf << file.rdbuf();

					uint64_t filenameSize = filename.size();
					archive.write((char*)&filenameSize, sizeof(uint64_t));
					archive.write(filename.c_str(), filename.size());


					uint64_t dataSize = buf.str().size();
					archive.write((char*)&dataSize, sizeof(uint64_t));
					archive << buf.str();
				}
				else {
					logError(SOURCE_LOCATION, "Script import stream is closed");
				}
			}
		}
	}



	void loadArchive(std::filesystem::path path)
	{
	}

	void readProjectFiles()
	{
		logInfo(SOURCE_LOCATION, "Read files from project directory:", this->projectFilesPath);

		for (const std::filesystem::directory_entry &entry :
			 std::filesystem::directory_iterator(this->projectFilesPath))
		{

			if (entry.is_directory())
			{
			}
			else
			{
				File *file = nullptr;

				if (auto it = projectFiles.find(entry.path().filename().string());
					it != projectFiles.end())
				{

					std::filesystem::file_time_type lastWriteTime = entry.last_write_time();

					file = projectFiles.at(it->first);

					if (lastWriteTime != it->second->lastWriteTime)
					{
						logInfo(SOURCE_LOCATION, "file", file->name, "modified");

						file->imported = false;
						file->lastWriteTime = lastWriteTime;
					}
				}
				else
				{
					logInfo(SOURCE_LOCATION, "First load:", entry);
					file = new File();
					file->name = entry.path().filename().string();
					file->extension = entry.path().filename().extension().string();
					file->lastWriteTime = std::filesystem::last_write_time(entry.path());
					projectFiles[file->name] = file;
				}

				std::ifstream f(entry.path(), std::ios::binary);
				if (f.is_open())
				{
					f.seekg(0, std::ios::end);
					std::streampos size = f.tellg();
					f.seekg(0, std::ios::beg);

					file->data.resize(size);
					f.read(file->data.data(), size);
				}
				else
				{
					logError(SOURCE_LOCATION, "Failed to open");
				}
			}
		}
	}

	~FileManager()
	{
	}

private:
	std::unordered_map<std::string, File *> projectFiles;
	std::filesystem::path projectFilesPath;
};