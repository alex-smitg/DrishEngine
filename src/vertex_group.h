#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "shader.h"
#include "transform.h"
#include "material.h"

class VertexGroup {
public:
	Shader* shader;
	Shader* depth_shader;
	unsigned int VBO, VAO;
	std::vector<GLfloat> vertices;
	Transform* transform;
	Material* material = nullptr;

	Transform temp;

	bool* draw_depth;

	//change it later;
	int vertex_stride = 14 * sizeof(GLfloat); //vertices = {{x, y, z, u, v, nx, ny, nz, tx, ty, tz, bx, by, bz}, ....} 

	VertexGroup(std::vector<GLfloat> data, Shader* shader, Shader* depth_shader, bool *draw_depth) {
		this->vertices = data;
		this->draw_depth = draw_depth;

		this->depth_shader = depth_shader;
		this->shader = shader;
		init();
	}

	~VertexGroup() {
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}


	int get_vertices_number() {
		return vertices.size() / 14;
	}

	void draw() {

		if (*draw_depth == true) {
			depth_shader->Use();
			depth_shader->setMat4("model", temp.get_matrix());
		}
		else {
			shader->Use();
			shader->setMat4("model", temp.get_matrix());
		}
		

		if (material != nullptr && *draw_depth == false) {
			shader->setFloat("normal_bump", material->normalmap_value);
			shader->setFloat("spec_factor", material->specular_value);
			shader->setFloat("reflection", material->reflection_value);
			shader->setFloat("ref_ratio", material->refract_value);
			shader->setBool("use_normal", material->use_normalmap);
			shader->setBool("use_specular", material->use_specular);
			shader->setFloat("shine", material->shine_value);
			shader->setFloat("dif", material->diffuse_value);
			shader->setVec3("diffuse_color", material->diffuse_color);
			shader->setBool("use_diffuse", material->use_diffuse);
			shader->setBool("use_blinn", material->use_blinn);
			shader->setVec2("uv_scale", material->uv_scale);
			shader->setInt("emit", material->emit);

			if (material->diffuse_texture != nullptr) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, material->diffuse_texture->id);
			}

			if (material->specular_texture != nullptr) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, material->specular_texture->id);
			}

			if (material->normalmap_texture != nullptr) {
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, material->normalmap_texture->id);
			}



		}

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 14);
	}

private:
	void init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

		vertex_stride = 14 * sizeof(GLfloat);
		//Position x,y,z
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_stride, 0);
		//Uv u,v
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(3 * sizeof(GLfloat)));
		//Normal nx, ny, nz
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(5 * sizeof(GLfloat)));
		//Tangent tx, ty, yz
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(8 * sizeof(GLfloat)));
		//Bitangent bx, by, bz (idk if it is bitangent)
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(11 * sizeof(GLfloat)));
	}
};