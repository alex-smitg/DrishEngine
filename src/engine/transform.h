#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "json.hpp"


class Transform {
public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f); 
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::mat4 global_matrix = glm::mat4(1.0f);

	glm::mat4 getMatrix() {
		glm::mat4 matrix(1.0f);

		matrix = glm::translate(matrix, position);

		matrix = glm::rotate(matrix, rotation.x, glm::vec3(1, 0, 0));
		matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1, 0));
		matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1));

		matrix = glm::scale(matrix, scale);

		global_matrix = matrix;

		return matrix;

	}

	glm::mat4 getMatrixWithParent(Transform* parent) {
		glm::mat4 matrix(1.0f);


		matrix = parent->global_matrix * getMatrix();
		global_matrix = matrix;

		return matrix;
	}
};

inline void to_json(nlohmann::json& j, const Transform& transform) {
	j["position"]["x"] = transform.position.x;
	j["position"]["y"] = transform.position.y;
	j["position"]["z"] = transform.position.z;
	j["rotation"]["x"] = transform.rotation.x;
	j["rotation"]["y"] = transform.rotation.y;
	j["rotation"]["z"] = transform.rotation.z;
	j["scale"]["x"] = transform.scale.x;
	j["scale"]["y"] = transform.scale.y;
	j["scale"]["z"] = transform.scale.z;
}

inline void from_json(const nlohmann::json& j, Transform& transform) {
	j.at("position").at("x").get_to(transform.position.x);
	j.at("position").at("y").get_to(transform.position.y);
	j.at("position").at("z").get_to(transform.position.z);
	j.at("rotation").at("x").get_to(transform.rotation.x);
	j.at("rotation").at("y").get_to(transform.rotation.y);
	j.at("rotation").at("z").get_to(transform.rotation.z);
	j.at("scale").at("x").get_to(transform.scale.x);
	j.at("scale").at("y").get_to(transform.scale.y);
	j.at("scale").at("z").get_to(transform.scale.z);
}