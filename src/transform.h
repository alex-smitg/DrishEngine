//__ReadAttributes__
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui_docking/imgui.h"

class Transform {
public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); //__Attribute__ vec3 position
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f); //__Attribute__ vec3 rotation
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f); //__Attribute__ vec3 scale


	glm::mat4 get_matrix() {
		glm::mat4 matrix(1.0f);
		matrix = glm::translate(matrix, position);

		matrix = glm::rotate(matrix, rotation.x, glm::vec3(1, 0, 0));
		matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1, 0));
		matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1));

		matrix = glm::scale(matrix, scale);

		return matrix;
	}
};