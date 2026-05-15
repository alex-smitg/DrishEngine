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
	AssetHandle verticesHandle;
	AssetHandle materialHandle;

	Model() {
		this->type = Type::MODEL; 

		addField(Field("Material", FieldType::MaterialHandle, &materialHandle));
		addField(Field("Vertices", FieldType::VerticesHandle, &verticesHandle));
	}
};

inline void to_json(nlohmann::json& j, const Model& model) {
	nlohmann::to_json(j, static_cast<Node>(model));
	j["materialAssId"] = model.materialHandle.index;
	j["verticesAssId"] = model.verticesHandle.index;
}