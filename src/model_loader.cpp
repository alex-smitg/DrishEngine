#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "model_loader.h"
#include <GL/glew.h>
#include <math.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib, "glew32s.lib")
#include <map>
#include <filesystem>






int open_obj(std::string path, std::vector<std::vector<GLfloat>>* vertices, std::vector<std::string>* mat_n) {
	if (!std::filesystem::exists(std::filesystem::path(path))) {
		std::cout << path << " not exists";
		return 0;
	}

	//std::wstring widestr = std::wstring(path.begin(), path.end());
	//const wchar_t* widecstr = widestr.c_str();

	std::ifstream file;
	file.open(path);
	if (!file) {
		std::cout << "error";
		return 0;
	}
	std::string str;
	std::vector<GLfloat> output;
	std::vector<GLfloat> temp_vertices;
	std::vector<GLfloat> tvertices;
	std::vector<std::string> all_faces;
	std::vector<GLfloat> normals;
	std::vector<std::vector<GLfloat>> real_output;
	std::vector<int> mat_delim;
	std::vector<std::string> mat_names;

	int vec_seek = 1;
	int not_first = 0;

	int real_seek = 0;

	bool last = false;

	std::string s;

	


	while (std::getline(file, str)) {
		if (str[0] == 'v' && str[1] == ' ') {
			//std::istringstream stream(str);
			std::string out = "";
			int size = str.size();

			for (std::string::size_type i = 2; i < size; i++) {
				if (!isspace(str[i])) {
					out += str[i];
				}
				if (isspace(str[i]) || i == size - 1) {
					temp_vertices.push_back((GLfloat)std::stof(out));
					out = "";
				}
			}
			//while (std::getline(stream, s, ' ')) {
				//if (s != "v") {
					//temp_vertices.push_back((GLfloat)std::stof(s));
				//}
			//}
		}

		if (str[0] == 'v' && str[1] == 't') {
			std::string out = "";
			int size = str.size();

			for (std::string::size_type i = 3; i < size; i++) {
				if (!isspace(str[i])) {
					out += str[i];
				}
				if (isspace(str[i]) || i == size - 1) {
					tvertices.push_back((GLfloat)std::stof(out));
					out = "";
				}
			}


			//while (std::getline(stream, s, ' ')) {
				//if (s != "vt") {
					//tvertices.push_back((GLfloat)std::stof(s));
				//}
			//}
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
					all_faces.push_back(out);
					out = "";
				}
			}
		}

		if (str[0] == 'u' && str[1] == 's') {
			mat_delim.push_back(all_faces.size());
			std::vector<GLfloat> vector;
			real_output.push_back(vector);

			std::istringstream stream(str);
			std::vector<std::string> words;

			while (std::getline(stream, s, ' ')) {
				if (s != "vn") {
					words.push_back(s);
				}
			}
			mat_names.push_back(words[1]);

		}
	}



	for (int i = 0; i < (all_faces.size() / 3); i += 1) {

		output.push_back(temp_vertices[(std::stoi(all_faces[i * 3]) - 1) * 3]);
		output.push_back(temp_vertices[(std::stoi(all_faces[i * 3]) - 1) * 3 + 1]);
		output.push_back(temp_vertices[(std::stoi(all_faces[i * 3]) - 1) * 3 + 2]);
		output.push_back(tvertices[(std::stoi(all_faces[i * 3 + 1]) - 1) * 2]);
		output.push_back(tvertices[(std::stoi(all_faces[i * 3 + 1]) - 1) * 2 + 1]);
		output.push_back(normals[(std::stoi(all_faces[i * 3 + 2]) - 1) * 3]);
		output.push_back(normals[(std::stoi(all_faces[i * 3 + 2]) - 1) * 3 + 1]);
		output.push_back(normals[(std::stoi(all_faces[i * 3 + 2]) - 1) * 3 + 2]);
		output.push_back(12.0);
		output.push_back(12.0);
		output.push_back(12.0);
		output.push_back(32.0);
		output.push_back(32.0);
		output.push_back(32.0);

	}



	for (int i = 0; i < output.size() / (14 * 3); i += 1) {
		GLfloat v1x = output[i * 14 * 3 + 14 * 0 + 0];
		GLfloat v1y = output[i * 14 * 3 + 14 * 0 + 1];
		GLfloat v1z = output[i * 14 * 3 + 14 * 0 + 2];
		GLfloat v1u = output[i * 14 * 3 + 14 * 0 + 3];
		GLfloat v1v = output[i * 14 * 3+ 14 * 0 + 4];

		GLfloat v2x = output[i * 14 * 3 + 14 * 1 + 0];
		GLfloat v2y = output[i * 14 * 3 + 14 * 1 + 1];
		GLfloat v2z = output[i * 14 * 3 + 14 * 1 + 2];
		GLfloat v2u = output[i * 14 * 3 + 14 * 1 + 3];
		GLfloat v2v = output[i * 14 * 3 + 14 * 1 + 4];

		GLfloat v3x = output[i * 14 * 3 + 14 * 2 + 0];
		GLfloat v3y = output[i * 14 * 3 + 14 * 2 + 1];
		GLfloat v3z = output[i * 14 * 3 + 14 * 2 + 2];
		GLfloat v3u = output[i * 14 * 3 + 14 * 2 + 3];
		GLfloat v3v = output[i * 14 * 3 + 14 * 2 + 4];

		

		glm::vec3 pos1 = glm::vec3(v1x, v1y, v1z);
		glm::vec3 pos2 = glm::vec3(v2x, v2y, v2z);
		glm::vec3 pos3 = glm::vec3(v3x, v3y, v3z);

		glm::vec2 uv1 = glm::vec2(v1u, v1v);
		glm::vec2 uv2 = glm::vec2(v2u, v2v);
		glm::vec2 uv3 = glm::vec2(v3u, v3v);

		

		glm::vec3 tangent, bitangent = glm::vec3(0.0, 0.0, 0.0);

		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		

		GLfloat f = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);





		if (f == INFINITY) {
			f = 0.0;
		}

		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);


		output[i * 14 * 3 + 14 * 0 + 8] = tangent.x;
		output[i * 14 * 3 + 14 * 0 + 9] = tangent.y;
		output[i * 14 * 3  + 14 * 0 + 10] = tangent.z;
		output[i * 14 * 3 + 14 * 0 + 11] = bitangent.x;
		output[i * 14 * 3 + 14 * 0 + 12] = bitangent.y;
		output[i * 14 * 3 + 14 * 0 + 13] = bitangent.z;

		output[i * 14 * 3 + 14 * 1 + 8] = tangent.x;
		output[i * 14 * 3 + 14 * 1 + 9] = tangent.y;
		output[i * 14 * 3 + 14 * 1 + 10] = tangent.z;
		output[i * 14 * 3 + 14 * 1 + 11] = bitangent.x;
		output[i * 14 * 3 + 14 * 1 + 12] = bitangent.y;
		output[i * 14 * 3 + 14 * 1 + 13] = bitangent.z;

		output[i * 14 * 3 + 14 * 2 + 8] = tangent.x;
		output[i * 14 * 3 + 14 * 2 + 9] = tangent.y;
		output[i * 14 * 3 + 14 * 2 + 10] = tangent.z;
		output[i * 14 * 3 + 14 * 2 + 11] = bitangent.x;
		output[i * 14 * 3+ 14 * 2 + 12] = bitangent.y;
		output[i * 14 * 3 + 14 * 2 + 13] = bitangent.z;





	}

	//std::cout << mat_delim[0] << "  " << mat_delim[1] << " ";
	
	if (mat_delim.size() != 0) {
		for (int i = 0; i < output.size(); i += 1) {
			if (vec_seek != mat_delim.size() && mat_delim[vec_seek] / 9 * 14 * 3 == i) {
				//std::cout << i << "\n";
				vec_seek += 1;



				if (vec_seek == mat_delim.size() - 1) {
					last = true;
				}

			}

			real_output[vec_seek - 1].push_back(output[i]);

		}
	}



	if (mat_delim.size() == 0) {
		vertices->push_back(output);
	}
	else {
		*vertices = real_output;
	}
	
	*mat_n = mat_names;
	return 1;


}


int open_mtl(std::string path, std::map<std::string, Mat> *materials) {
	if (!std::filesystem::exists(path)) {
		std::cout << path << " not exist - open_mtl()" << "\n";
		return -1;
	}

	std::ifstream file;
	file.open(path);
	if (!file) {
		std::cout << path;
		std::cout << "error";
		return -1;
	}

	std::string str;

	std::map<std::string, Mat> mat;

	std::string current_mat;

	while (std::getline(file, str)) {
		if (str[0] == 'n' && str[1] == 'e') {
			std::istringstream stream(str);
			std::vector<std::string> words;
			std::string s;
			while (std::getline(stream, s, ' ')) {
				words.push_back(s);
			}
			current_mat = words[1];
			Mat mater;
			mat[current_mat] = mater;
;		}



		if (str[0] == 'm') {
			std::istringstream stream(str);
			std::vector<std::string> words;
			std::string s;
			while (std::getline(stream, s, ' ')) {
				words.push_back(s);
			}

			std::filesystem::path new_path = std::filesystem::u8path(words[1]);
			mat[current_mat].dif_path = "images/" + new_path.filename().string();
			//std::cout << mat[current_mat] << "\n";
		}

		if (str[0] == 'K' && str[1] == 'd') {
			std::istringstream stream(str);
			std::vector<float> words;
			std::string s;
			while (std::getline(stream, s, ' ')) {
				if (s != "Kd") {
					words.push_back((GLfloat)std::stof(s));
				}
			}

			mat[current_mat].d_color = glm::vec3(words[0], words[1], words[2]);
		}
	}

	*materials = mat;
	return 1;
}

