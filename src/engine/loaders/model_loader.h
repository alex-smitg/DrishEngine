#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include "logger.h"
#include "assets/vertices.h"



namespace drishengine {
	static int loadObj(std::filesystem::path path, Vertices* vertices) {
		if (!std::filesystem::exists(path)) {
			logError("[MODEL LOADER] ", path, " where?");
			return 0;
		}

		std::ifstream file(path);

		std::vector<float> positions; 
		std::vector<float> normals;
		std::vector<float> textures; //uv coordinates
		std::vector<std::string> faces;

		std::string str;

		while (std::getline(file, str)) {
			if (str[0] == 'v' && str[1] == ' ') {
				std::string out = "";
				int size = str.size();

				for (std::string::size_type i = 2; i < size; i++) {
					if (!isspace(str[i])) {
						out += str[i];
					}
					if (isspace(str[i]) || i == size - 1) {
						positions.push_back((float)std::stof(out));
						out = "";
					}
				}
			}

			if (str[0] == 'v' && str[1] == 't') {
				std::string out = "";
				int size = str.size();

				for (std::string::size_type i = 3; i < size; i++) {
					if (!isspace(str[i])) {
						out += str[i];
					}
					if (isspace(str[i]) || i == size - 1) {
						textures.push_back((float)std::stof(out));
						out = "";
					}
				}
			}

			if (str[0] == 'v' && str[1] == 'n') {
				std::string out = "";
				int size = str.size();

				for (std::string::size_type i = 3; i < size; i++) {
					if (!isspace(str[i])) {
						out += str[i];
					}
					if (isspace(str[i]) || i == size - 1) {
						normals.push_back((GLfloat)std::stof(out));
						out = "";
					}
				}
			}

			if (str[0] == 'f') {
				std::string out = "";
				int size = str.size();

				for (std::string::size_type i = 2; i < size; i++) {
					if (str[i] != '/' && !isspace(str[i])) {
						out += str[i];
					}
					if (isspace(str[i]) || i == size - 1 || str[i] == '/') {
						faces.push_back(out);
						out = "";
					}
				}
			}
		}

		for (int i = 0; i < (faces.size() / 3); i += 1) {
			vertices->data.push_back(positions[(std::stoi(faces[i * 3]) - 1) * 3]);
			vertices->data.push_back(positions[(std::stoi(faces[i * 3]) - 1) * 3 + 1]);
			vertices->data.push_back(positions[(std::stoi(faces[i * 3]) - 1) * 3 + 2]);
			vertices->data.push_back(textures[(std::stoi(faces[i * 3 + 1]) - 1) * 2]);
			vertices->data.push_back(textures[(std::stoi(faces[i * 3 + 1]) - 1) * 2 + 1]);
			vertices->data.push_back(normals[(std::stoi(faces[i * 3 + 2]) - 1) * 3]);
			vertices->data.push_back(normals[(std::stoi(faces[i * 3 + 2]) - 1) * 3 + 1]);
			vertices->data.push_back(normals[(std::stoi(faces[i * 3 + 2]) - 1) * 3 + 2]);
		}


		return 0;
	}

	static void writeModelData(std::filesystem::path path, std::vector<float> data) {
		logDebug("[MODEL LOADER] ", "write data: ", path.string());
		
		std::ofstream file(path, std::ios::out | std::ios::binary);

		int size = data.size();
		file.write(reinterpret_cast<const char*>(&size), sizeof(size));

		for (float f : data) {
			file.write(reinterpret_cast<const char*>(&f), sizeof(f));
		}
	}

	static void loadModelData(std::filesystem::path path, std::vector<float>* data) {
		std::ifstream file(path, std::ios::in | std::ifstream::binary);

		int size = 0;
		file.read(reinterpret_cast<char*>(&size), sizeof(size));
		data->reserve(size);
		for (int i = 0; i < size; i++) {
			float f = 0.0;
			file.read(reinterpret_cast<char*>(&f), sizeof(f));
			data->push_back(f);
		}
	}
}