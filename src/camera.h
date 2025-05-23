#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Camera {
public:
	glm::mat4 view;
	glm::vec3 position;
	float fov = 75;

	Camera() {
		view = glm::mat4(1.0);
		position = glm::vec3(0.0);
	}
};


