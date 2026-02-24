#pragma once

#include <vector>
#include <string>
#include <map>

#include "node.h"
#include "../transform.h"
#include "../assets/vertices.h"
#include "../assets/material.h"

class Model: public Node {
public:
	Material* material = nullptr;
	Vertices* vertices = nullptr;

	Model() {
		this->type = Type::MODEL; 

		addField(Field("Material", FieldType::MaterialClass, 1, &material));
		addField(Field("Vertices", FieldType::VerticesClass, 1, &vertices));
	}
};

inline void to_json(nlohmann::json& j, const Model& model) {
	nlohmann::to_json(j, static_cast<Node>(model));
	if (model.material != nullptr) {
		j["materialAssId"] = model.material->assId;
	}
	if (model.vertices != nullptr) {
		j["verticesAssId"] = model.vertices->assId;
	}
}