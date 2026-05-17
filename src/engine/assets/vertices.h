#pragma once

#include <vector>
#include <string>

#include "asset.h"



class Vertices : public Asset {
public:
	std::vector<float> data;

	unsigned int VBO, VAO;

	const int BUFFER_STRIDE = 8;

	std::filesystem::path path;

	Vertices() {
		type = AssetType::VERTICES;
	}

	~Vertices() {
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &VAO);
	}

	void draw() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, data.size() / BUFFER_STRIDE);
	}

	void createBuffers() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

		int floatStride = BUFFER_STRIDE * sizeof(float);
		//Position x,y,z
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, floatStride, 0);
		//Texture u,v
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, floatStride, (void*)(3 * sizeof(GLfloat)));
		//Normal nx, ny, nz
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, floatStride, (void*)(5 * sizeof(GLfloat)));

	}
};

inline void to_json(nlohmann::json& j, const Vertices& vertices) {
	nlohmann::to_json(j, static_cast<Asset>(vertices));
	j["path"] = vertices.path;
}
