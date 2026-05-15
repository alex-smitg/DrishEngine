#pragma once

#include "node.h"
#include "../transform.h"
#include "../line.h"

class PointLight : public Node {
public:
	glm::vec3 color = {1.0, 1.0, 1.0};
	float strength = 1.0;
	float radius = 1.0;

	PointLight() {
		this->type = Type::POINT_LIGHT;

		addField(Field("Color", FieldType::Color3, &color));
		addField(Field("Strength", FieldType::Float, &strength));
		addField(Field("Radius", FieldType::Float, &radius));
	}

	void update(double delta) override {
		glm::mat4 model;

		model = transform.getMatrix();
		
		line1.model = model;
		line2.model = model;
		line3.model = model;

		line1.color = color;
		line2.color = color;
		line3.color = color;

		Node::update(delta);
	}


	void drawInEditor(Shader* shader) override {
		line1.draw(shader);
		line2.draw(shader);
		line3.draw(shader);


		Node::drawInEditor(shader);
	}

private:
	Line line1 = Line(glm::vec3(-0.5, 0.0, 0.0),
		glm::vec3(0.5, 0.0, 0.0),
		glm::vec3(1.0, 1.0, 1.0));
	Line line2 = Line(glm::vec3(0.0, -0.5, 0.0),
		glm::vec3(0.0, 0.5, 0.0),
		glm::vec3(1.0, 1.0, 1.0));
	Line line3 = Line(glm::vec3(0.0, 0.0, -0.5),
		glm::vec3(0.0, 0.0, 0.5),
		glm::vec3(1.0, 1.0, 1.0));
};


inline void to_json(nlohmann::json& j, const PointLight& pointLight) {
	nlohmann::to_json(j, static_cast<Node>(pointLight));
	j["color"]["r"] = pointLight.color.r;
	j["color"]["g"] = pointLight.color.g;
	j["color"]["b"] = pointLight.color.b;
	j["strength"] = pointLight.strength;
	j["radius"] = pointLight.radius;
}