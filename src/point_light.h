#pragma once


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "transform.h"
#include "base_object.h"
#include "shader.h"
#include "light_model.h"


class PointLight : public BaseObject {
public:
	glm::vec3 color;
	GLfloat strength;
	Shader* shader;
	unsigned int VBO, VAO;

	bool visible = true;

	float radius = 3.0;

	PointLight(Shader* shader, std::string name, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0), GLfloat strength = 1.0) : BaseObject() {
		this->strength = strength;
		this->transform.scale = glm::vec3(0.2f, 0.2f, 0.2f);
		this->color = color;
		this->shader = shader;
		this->name = name;

		//test

		//sol::usertype<PointLight> player_type = lua.new_usertype<PointLight>(name,
		//sol::constructors<PointLight(Shader*, std::string, glm::vec3, GLfloat)>());
		properties.push_back(Property::TRANSFORM);
		properties.push_back(Property::LIGHT);

		type = POINT_LIGHT;
		init();
	};


	void draw() override {
		shader->Use();
		shader->setVec3("color", color);

		Transform temp;
		temp.position.x = parent->transform.position.x + transform.position.x;
		temp.position.y = parent->transform.position.y + transform.position.y;
		temp.position.z = parent->transform.position.z + transform.position.z;

		temp.scale = transform.scale;
		temp.rotation = transform.rotation;

		shader->setMat4("model", temp.get_matrix());
		shader->setFloat("strenght", strength);

		if (visible) {
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, low_poly_sphere.size() / 3);
		}


	}

	void update() override {

		if (!script.empty()) {
			/*lua["pos_x"] = model[3].x;
			lua["pos_y"] = model[3].y;
			lua["pos_z"] = model[3].z;*/
			lua["r"] = color.r;
			lua["g"] = color.g;
			lua["b"] = color.b;

			try {
				lua.script(script);
			}
			catch (const std::exception& ex) {
				std::cout << ex.what();
				script = "";
			}

			/*model[3].x = lua["pos_x"];
			model[3].y = lua["pos_y"];
			model[3].z = lua["pos_z"];*/
			color.r = lua["r"];
			color.g = lua["g"];
			color.b = lua["b"];

			color.r = std::clamp(color.r, 0.0f, 1.0f);
			color.g = std::clamp(color.g, 0.0f, 1.0f);
			color.b = std::clamp(color.b, 0.0f, 1.0f);
		}
	}

private:
	void init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);


		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, low_poly_sphere.size() * sizeof(GLfloat), &low_poly_sphere[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
};