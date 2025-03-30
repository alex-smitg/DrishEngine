#pragma once

#include "property_drawer.h"

#include "mesh_holder.h"
#include "base_object.h"
#include "point_light.h"
#include "rigidbody.h"
#include "directional_light.h"


#include "imgui_docking/imgui.h"




void registerProperties(PropertyDrawer *drawer) {
	drawer->registerDrawer<BaseObject>([](BaseObject& obj) {
		ImGui::SeparatorText(obj.name.c_str());
		
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
		if (ImGui::TreeNode("Transform")) {
			ImGui::SeparatorText("Position");
			ImGui::PushItemWidth(64.0f);
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::DragFloat("##x", &(obj.transform.position.x), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::DragFloat("##y", &(obj.transform.position.y), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::DragFloat("##z", &(obj.transform.position.z), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::SeparatorText("Rotation");
			ImGui::PushItemWidth(64.0f);
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::DragFloat("##rx", &(obj.transform.rotation.x), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::DragFloat("##ry", &(obj.transform.rotation.y), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::DragFloat("##rz", &(obj.transform.rotation.z), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::SeparatorText("Scale");
			ImGui::PushItemWidth(64.0f);
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::DragFloat("##sx", &(obj.transform.scale.x), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::DragFloat("##sy", &(obj.transform.scale.y), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::DragFloat("##sz", &(obj.transform.scale.z), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::TreePop();
			}
		

		if (ImGui::TreeNode("Script")) {
			ImGui::Text("script");

			ImGui::TreePop();
		}

		ImGui::PopStyleVar();
	});

		
	drawer->registerDrawer<MeshHolder>([&drawer](BaseObject& obj) {
		drawer->draw(&typeid(BaseObject), &obj);

		MeshHolder *meshHolder = static_cast<MeshHolder*>(&obj);

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
		if (ImGui::TreeNode("Mesh")) {
			std::string text = "Empty";
			if (meshHolder->mesh != nullptr) {
				text = "Mesh";
			}
			ImGui::Button(text.c_str(), { 64, 64 });

			ImGui::TreePop();
		}


		ImGui::PopStyleVar();


	});

	drawer->registerDrawer<PointLight>([&drawer](BaseObject& obj) {
		drawer->draw(&typeid(BaseObject), &obj);
		PointLight* pointLight = static_cast<PointLight*>(&obj);

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
		if (ImGui::TreeNode("Light")) {
			ImGui::SeparatorText("Light");
			ImGui::SliderFloat("Radius", &(pointLight->radius), 0.0, 50.0);
			ImGui::SliderFloat("Strength", &(pointLight->strength), 0.0, 50.0);
			ImGui::Separator();

			float color[3];

			color[0] = pointLight->color.x;
			color[1] = pointLight->color.y;
			color[2] = pointLight->color.z;

			ImGui::ColorPicker3("Light Color", color);

			pointLight->color = glm::vec3(color[0], color[1], color[2]);

			ImGui::TreePop();
		}


		ImGui::PopStyleVar();


	});

	drawer->registerDrawer<DirectionalLight>([&drawer](BaseObject& obj) {
		drawer->draw(&typeid(BaseObject), &obj);

		DirectionalLight* dirLight = static_cast<DirectionalLight*>(&obj);


	});

	drawer->registerDrawer<RigidBody>([&drawer](BaseObject& obj) {
		drawer->draw(&typeid(BaseObject), &obj);


	});
}

