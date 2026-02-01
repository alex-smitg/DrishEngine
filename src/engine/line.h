#pragma once


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "transform.h"
#include "shader.h"

#include <vector>



class Line {
public:
	unsigned int VBO, VAO;
	glm::vec3 color;
	std::vector<GLfloat> verts;
	glm::mat4 model;

	bool visible = true;


	Line(glm::vec3 start_pos, glm::vec3 end_pos, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0)) {
		this->color = color;
		verts.push_back(start_pos.x);
		verts.push_back(start_pos.y);
		verts.push_back(start_pos.z);
		verts.push_back(end_pos.x);
		verts.push_back(end_pos.y);
		verts.push_back(end_pos.z);
		init();
	};

	void draw(Shader* shader) {
		shader->use();
		shader->setVec3("color", color);
		shader->setMat4("model", model);
		shader->setInt("useLight", 0);

		if (visible) {
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINES, 0, 2);
		}

	}

	void update_geometry() {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_DYNAMIC_DRAW);
	}



private:
	void init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);


		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

};