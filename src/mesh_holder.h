#pragma once

#include "base_object.h"
#include "mesh.h"
#include "shader.h"

#include "types.h"

class MeshHolder : public BaseObject {
public:
	Shader* shader;
	Mesh* mesh = nullptr;


	MeshHolder(Shader* shader, std::string name) : BaseObject() {
		this->name = name;
		this->shader = shader;
		type = MESH_HOLDER;

		properties.push_back(Property::TRANSFORM);
		//properties.push_back(Property::TEXTURE);
	}

	~MeshHolder() {
	}




	void draw() override {
		if (mesh != nullptr) {
			shader->Use();

			Transform temp;
			temp.position.x = parent->transform.position.x + transform.position.x;
			temp.position.y = parent->transform.position.y + transform.position.y;
			temp.position.z = parent->transform.position.z + transform.position.z;

			temp.scale = transform.scale;
			temp.rotation = transform.rotation;


			mesh->temp = temp;
			mesh->transform = &transform;
			mesh->draw();
		}


	}


};

