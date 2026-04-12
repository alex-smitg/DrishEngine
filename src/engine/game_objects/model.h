#pragma once

#include <vector>
#include <string>
#include <map>

#include "node.h"
#include "../transform.h"
#include "../assets/vertices.h"
#include "../assets/material.h"

class Model: public Node {
private:
	//editor/user_interface/properties_window.h still can edit this field. I don't know how to fix this.
	Material* material_ = nullptr;

public:
	Vertices* vertices = nullptr;


	
	Material* getMaterial() const {
		return material_;
	}

	
	void assignMaterial(Material* material) {
		if (material == nullptr) {
			material = nullptr;
		}
		else if (material != this->material_) {
			this->material_ = material;
		}
	}

	bool hasMaterial() const {
		return (this->material_ != nullptr);
	}


	Model() {
		this->type = Type::MODEL; 

		addField(Field("Material", FieldType::MaterialClass, 1, &material_));
		addField(Field("Vertices", FieldType::VerticesClass, 1, &vertices));
	}
private:

};

inline void to_json(nlohmann::json& j, const Model& model) {
	nlohmann::to_json(j, static_cast<Node>(model));
	if (model.hasMaterial()) {
		j["materialAssId"] = model.getMaterial()->assId;
	}
	if (model.vertices != nullptr) {
		j["verticesAssId"] = model.vertices->assId;
	}
}