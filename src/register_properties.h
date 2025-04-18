#pragma once

#include "property_drawer.h"

#include "mesh_holder.h"
#include "base_object.h"
#include "point_light.h"
#include "rigidbody.h"
#include "directional_light.h"
#include "bezier.h"

#include "texture.h"

#include "imgui_docking/imgui.h"
#include "imgui_docking/imgui_stdlib.h"



void drawTexture(Texture *texture) {

}


void registerProperties(PropertyDrawer *drawer) {
	drawer->registerDrawer<BaseObject>([](BaseObject& obj) {
		ImGui::SeparatorText(obj.name.c_str());

		if (obj.name != "root" && ImGui::TreeNode("Actions")) {
			ImGui::SeparatorText("Actions");
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.639f, 0.153f, 0.153f, 1.0f));
			if (ImGui::Button("Delete")) {}
			ImGui::PopStyleColor();

			if (ImGui::Button("Rename")) {}
			ImGui::TreePop();
		}
		
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
			std::string script = obj.script;
			static std::string temp_script = script;
			static std::string error = "";

			ImGui::Text(obj.script.c_str());

			ImGui::InputTextMultiline("##00", &temp_script, ImVec2(0, 0), 0);

			if (ImGui::Button("Apply")) {
				error = obj.set_script(temp_script);

			}

			ImGui::PushStyleColor(0, ImVec4(1.0, 0.0, 0.0, 1.0));
			ImGui::Text(error.c_str());
			ImGui::PopStyleColor();


			ImGui::TreePop();
		}

		ImGui::PopStyleVar();
	});

		
	drawer->registerDrawer<MeshHolder>([&drawer](BaseObject& obj) {
		drawer->draw(&typeid(BaseObject), &obj);

		MeshHolder *meshHolder = static_cast<MeshHolder*>(&obj);

		static int selected_item = 0;
		static VertexGroup* svg = nullptr;

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
		if (ImGui::TreeNode("Mesh")) {
			std::string text = "Empty";
			if (meshHolder->mesh != nullptr) {
				text = meshHolder->mesh->name + " " + std::to_string(meshHolder->mesh->vertice_number);
			}
			ImGui::Button(text.c_str(), { 64, 64 });
			ImGui::SameLine();
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESH"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int mesh_i = *(const int*)payload->Data;

					//obj->mesh = resourceManager.meshes[mesh_i];

				}
				ImGui::EndDragDropTarget();
			}

			if (meshHolder->mesh != nullptr) {
				if (ImGui::BeginListBox("##Materials", ImVec2(0, 50))) {
					for (int n = 0; n < meshHolder->mesh->vertexGroups.size(); n++)
					{
						const bool is_selected = (selected_item == n);
						if (ImGui::Selectable((meshHolder->mesh->vertexGroups[n]->material->name + "##" + std::to_string(n)).c_str(), is_selected)) {
							selected_item = n;
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
							svg = meshHolder->mesh->vertexGroups[selected_item];
						}

					}
					ImGui::EndListBox();
				}
			}

		
			if (svg && meshHolder->mesh != nullptr) {
				ImGui::Text("test");

				if (svg->material != nullptr) {
					Material* material = svg->material;

					ImGui::SliderFloat("uv x", &(svg->material->uv_scale.x), 0.0, 25.0);
					ImGui::SliderFloat("uv y", &(svg->material->uv_scale.y), 0.0, 25.0);

					//Diffuse
					{
						ImGui::BeginGroup();
						ImGui::Text("Diffuse");
						ImGui::Checkbox("Use texture", &(svg->material->use_diffuse)); ImGui::SameLine();
						ImGui::Checkbox("Emit", &(svg->material->emit)); ImGui::SameLine();

						float col[3] = { svg->material->diffuse_color.r, svg->material->diffuse_color.g, svg->material->diffuse_color.b };
						ImGui::SetColorEditOptions(ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
						ImGui::ColorEdit3("Color", col);
						svg->material->diffuse_color = glm::vec3(col[0], col[1], col[2]);

						ImGui::SliderFloat("##diff", &(svg->material->diffuse_value), 0.0, 1.0);

						if (material->diffuse_texture != nullptr) {
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
							ImGui::ImageButton(("id##diff" + std::to_string(material->diffuse_texture->id)).c_str(), material->diffuse_texture->id,
								ImVec2(64, 64), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0, 0.0, 0.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0));

							ImGui::PopStyleVar();
						}
						else {
							ImGui::Button("Texture", { 64,64 });
						}
						if (ImGui::BeginDragDropTarget()) {
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE")) {
								IM_ASSERT(payload->DataSize == sizeof(int));
								int texture_i = *(const int*)payload->Data;

								svg->material->use_diffuse = true;

								//svg->material->diffuse_texture = resourceManager.textures[texture_i];

							}
							ImGui::EndDragDropTarget();
						}
						ImGui::EndGroup();
					}
					//Specular
					{
						ImGui::BeginGroup();
						ImGui::Text("Specular");
						ImGui::Checkbox("Use texture##spec", &(svg->material->use_specular)); ImGui::SameLine();

						ImGui::SliderFloat("##spec", &(svg->material->specular_value), 0.0, 1.0);

						if (material->specular_texture != nullptr) {
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
							ImGui::ImageButton(("id##spec" + std::to_string(material->specular_texture->id)).c_str(), material->specular_texture->id,
								ImVec2(64, 64), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0, 0.0, 0.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0));

							ImGui::PopStyleVar();
						}
						else {
							ImGui::Button("Texture##spec", { 64,64 });
						}
						if (ImGui::BeginDragDropTarget()) {
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE")) {
								IM_ASSERT(payload->DataSize == sizeof(int));
								int texture_i = *(const int*)payload->Data;

								svg->material->use_specular = true;

								//svg->material->specular_texture = resourceManager.textures[texture_i];

							}
							ImGui::EndDragDropTarget();
						}
						ImGui::EndGroup();
					}
					//normal
					{
						ImGui::BeginGroup();
						ImGui::Text("Normal");
						ImGui::Checkbox("Use texture##normal", &(material->use_normalmap));
						ImGui::SameLine();
						ImGui::SliderFloat("##normal", &(svg->material->normalmap_value), 0.0, 1.0);

						if (material->normalmap_texture != nullptr) {
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
							ImGui::ImageButton(("id##normal" + std::to_string(material->normalmap_texture->id)).c_str(), material->normalmap_texture->id,
								ImVec2(64, 64), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0, 0.0, 0.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0));

							ImGui::PopStyleVar();
						}
						else {
							ImGui::Button("Texture##normal", { 64,64 });
						}
						if (ImGui::BeginDragDropTarget()) {
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE")) {
								IM_ASSERT(payload->DataSize == sizeof(int));
								int texture_i = *(const int*)payload->Data;

								svg->material->use_normalmap = true;

								//svg->material->normalmap_texture = resourceManager.textures[texture_i];

							}
							ImGui::EndDragDropTarget();
						}
						ImGui::EndGroup();
					}

					ImGui::DragFloat("Shine", &(material->shine_value));
					ImGui::DragFloat("Refract", &(material->refract_value), 0.1f);
					ImGui::DragFloat("Reflect", &(material->reflection_value), 0.1f);

					static int e = 0;
					ImGui::RadioButton("Phong", &e, 0); ImGui::SameLine();
					ImGui::RadioButton("Blinn", &e, 1);

					svg->material->use_blinn = e;

				}
				else {
					ImGui::Text("No material");
				}

			}
 


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

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
		if (ImGui::TreeNode("Light")) {
			ImGui::SeparatorText("Light");
			ImGui::PushItemWidth(64.0f);
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::DragFloat("##xd", &(dirLight->direction.x), 0.1f, -2.0f, 2.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::DragFloat("##yd", &(dirLight->direction.y), 0.1f, -2.0, 2.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::DragFloat("##zd", &(dirLight->direction.z), 0.1f, -2.0f, 2.0f, "%.2f");
			ImGui::SliderFloat("Strength", &(dirLight->strength), 0.0, 50.0);
			ImGui::Separator();

			float color[3];

			color[0] = dirLight->color.x;
			color[1] = dirLight->color.y;
			color[2] = dirLight->color.z;

			ImGui::ColorPicker3("Light Color", color);

			dirLight->color = glm::vec3(color[0], color[1], color[2]);

			ImGui::TreePop();
		}


		ImGui::PopStyleVar();


	});

	drawer->registerDrawer<RigidBody>([&drawer](BaseObject& obj) {
		drawer->draw(&typeid(BaseObject), &obj);


	});

	drawer->registerDrawer<Bezier>([&drawer](BaseObject& obj) {
		Bezier* curve = static_cast<Bezier*>(&obj);
		drawer->draw(&typeid(BaseObject), &obj);

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
		if (ImGui::TreeNode("Curve")) {
			ImGui::PushItemWidth(64.0f);
			ImGui::DragFloat("X1", &(curve->p1.x), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::DragFloat("Y1", &(curve->p1.y), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::DragFloat("Z1", &(curve->p1.z), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Separator();
			ImGui::DragFloat("X2", &(curve->p2.x), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::DragFloat("Y2", &(curve->p2.y), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::DragFloat("Z2", &(curve->p2.z), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Separator();
			ImGui::DragFloat("X3", &(curve->p3.x), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::DragFloat("Y3", &(curve->p3.y), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::DragFloat("Z3", &(curve->p3.z), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Separator();

			ImGui::TreePop();
		}
		ImGui::PopStyleVar();
	});
}

