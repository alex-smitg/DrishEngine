#pragma once

#include "base_object.h"
#include "mesh.h"
#include "shader.h"

#include "types.h"

#include "imgui_docking/imgui.h"

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


	void draw_properties() override {
		BaseObject::draw_properties();

		if (ImGui::TreeNode("Mesh")) {
			ImGui::Button("empty", { 64, 64 });
			ImGui::SameLine();
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESH"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int mesh_i = *(const int*)payload->Data;

					//mesh = resourceManager.meshes[mesh_i];

				}
				ImGui::EndDragDropTarget();
			}

			ImGui::ShowDemoWindow();

			if (mesh != nullptr) {
				mesh->draw_properties();
			}

			ImGui::TreePop();
		}
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

