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

	

	/*void drawProperties(AssetRepository* assetRepository, std::filesystem::path projectPath) override {
		Node::drawProperties(assetRepository, projectPath);
		ImGui::Separator();
		if (ImGui::TreeNodeEx("Vertices", ImGuiTreeNodeFlags_Framed)) {
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("VERTICES")) {
					long long payload_assid = *(long long*)payload->Data;
					this->vertices = assetRepository->getVertices(payload_assid);
				}
				ImGui::EndDragDropTarget();
			}
			if (vertices == nullptr) {
				if (ImGui::Button("Load")) {
					std::filesystem::path objFilePath = drishengine::openModelOpenFileDialog();

					if (!objFilePath.empty()) {
						std::filesystem::path filename = objFilePath.filename();
						filename.replace_extension("model");

						vertices = AssetCreator::createVertices(objFilePath.stem().string(), -1);
						
						vertices->path = std::filesystem::path("models") / filename;
						drishengine::loadObj(objFilePath, vertices);
						assetRepository->addVertices(vertices);

						if (std::filesystem::exists(projectPath / "models")) {
							LOG_DEBUG("[MODEL] models exists");
						}
						else {
							std::filesystem::create_directory(projectPath / "models");
						}

						

						drishengine::writeModelData(projectPath / "models" / filename,
							vertices->data);
						vertices->createBuffers();
						this->vertices = vertices;
					}
				}
			}
			if (vertices != nullptr) {
				ImGui::Text(vertices->name.c_str());
				ImGui::Text("size %d", vertices->data.size());
				if (ImGui::Button("-")) {
					vertices = nullptr;
				}
			}
			ImGui::TreePop();
		}
		ImGui::Separator();

		if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_Framed)) {
			ImGui::Button("Drop material here");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIAL")) {
					long long payload_assid = *(long long*)payload->Data;
					this->material = assetRepository->getMaterial(payload_assid);
				}
				ImGui::EndDragDropTarget();
			}
			if (material == nullptr || material->name == "default") {
				ImGui::Text("No material");
				if (ImGui::Button("+")) {
					material = AssetCreator::createMaterial("mat");
					material->shader = &assetRepository->defaultShader;
				}
				
			}
			
			else {
				ImGui::Text(material->name.c_str());
				float col[3] = { material->color.x, material->color.y, material->color.z };
				ImGui::ColorEdit3("Color", col);
				material->color.x = col[0];
				material->color.y = col[1];
				material->color.z = col[2];

				
				
				if (ImGui::TreeNode("Texture")) {
					ImGui::Button("Drop texture here");
					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE")) {
							long long payload_assid = *(long long*)payload->Data;
							this->material->texture = assetRepository->getTexture(payload_assid);
						}
						ImGui::EndDragDropTarget();

					}

					if (material->texture == nullptr) {
						ImGui::Text("No texture");
						if (ImGui::Button("Load")) {
							std::filesystem::path image_path = drishengine::openImageOpenFileDialog();
							Texture* texture = AssetCreator::createTexture(image_path.filename().generic_string());
							texture->path = std::filesystem::path("textures") / image_path.filename();
							
							if (std::filesystem::exists(projectPath / "textures")) {
								LOG_DEBUG("[MODEL] texutures exists");
							}
							else {
								std::filesystem::create_directory(projectPath / "textures");
							}
							
							LOG_DEBUG("[MODEL] texture name: ", texture->name);
							LOG_DEBUG("[MODEL] texture path: ", texture->path);
							LOG_DEBUG("[MODEL] to: ", projectPath / texture->path);
							ImageLoader::loadImage(image_path, texture);
							material->texture = texture;

							if (image_path == (projectPath / texture->path)) {
								LOG_WARNING("[MODEL] Paths are same");
							} else {
								std::ifstream from(image_path, std::ios::binary);
								std::ofstream to(projectPath / texture->path, std::ios::binary);

								to << from.rdbuf();

								from.close();
								to.close();
							}
						}
					}
					else {
						ImGui::Image(material->texture->id, ImVec2(64, 64));
						if (ImGui::Button("-")) {
							material->texture = nullptr;
						}
					}

					ImGui::TreePop();
				}
			}
			if (material != nullptr && material->name != "default") {
				ImGui::Text(material->name.c_str());
				if (ImGui::Button("-")) {
					material = nullptr;
				}
			}
			ImGui::TreePop();
		}
	}*/

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