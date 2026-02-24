#pragma once

#include "node.h"
#include "../transform.h"
#include "../line.h"


class Camera : public Node {
public:
	float fov = 45.0f;
	float f_near = 0.1f;
	float f_far = 1000.0f;
	float windowAspectRatio = 1.0f;

	glm::mat4 view = glm::mat4(1.0f);

	glm::mat4 perspective = glm::perspective(glm::radians(fov), windowAspectRatio, f_near, f_far);

	bool active = true;

	bool create_view = true;

	Camera() {
		this->type = Type::CAMERA;
		addField(Field("Fov", FieldType::Float, 1, &fov));
		addField(Field("Near", FieldType::Float, 1, &f_near));
		addField(Field("Far", FieldType::Float, 1, &f_far));
		addField(Field("Active", FieldType::Boolean, 1, &active));
	}

	void setWindowAspectRatio(float windowAspectRatio) {
		this->windowAspectRatio = windowAspectRatio;
		this->perspective = glm::perspective(glm::radians(fov), windowAspectRatio, f_near, f_far);
	}

	void setWindowAspectRatio(float width, float height) {
		this->windowAspectRatio = width / height;
		this->perspective = glm::perspective(glm::radians(fov), this->windowAspectRatio, f_near, f_far);
	}


	void update(double delta) override {
		glm::mat4 model;

		model = transform.getMatrix();
		

		line1.model = model;
		line2.model = model;
		line3.model = model;
		line4.model = model;
		line5.model = model;

		Node::update(delta);
	}

	void drawInEditor(Shader* shader) override {
		line1.draw(shader);
		line2.draw(shader);
		line3.draw(shader);
		line4.draw(shader);
		line5.draw(shader);

		Node::drawInEditor(shader);
	}

private:
	Line line1 = Line(glm::vec3(0.0, 0.0, 0.0),
		glm::vec3(0.5, 0.0, 0.0),
		glm::vec3(1.0, 0.0, 0.0));
	Line line2 = Line(glm::vec3(0.0, 0.0, 0.0),
		glm::vec3(0.0, 0.5, 0.0),
		glm::vec3(1.0, 0.0, 0.0));
	Line line3 = Line(glm::vec3(0.0, 0.5, 0.0),
		glm::vec3(0.5, 0.5, 0.0),
		glm::vec3(1.0, 0.0, 0.0));
	Line line4 = Line(glm::vec3(0.5, 0.5, 0.0),
		glm::vec3(0.5, 0.0, 0.0),
		glm::vec3(1.0, 0.0, 0.0));
	Line line5 = Line(glm::vec3(0.0, 0.0, 0.0),
		glm::vec3(0.0, 0.0, 1.0),
		glm::vec3(1.0, 0.0, 0.0));
};


inline void to_json(nlohmann::json& j, const Camera& camera) {
	nlohmann::to_json(j, static_cast<Node>(camera));
	j["fov"] = camera.fov;
	j["f_near"] = camera.f_near;
	j["f_far"] = camera.f_far;
	j["active"] = camera.active;
}