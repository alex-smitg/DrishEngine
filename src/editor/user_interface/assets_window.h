#pragma once

#include <string>

#include "imgui_docking/imgui.h"
#include "imgui_docking/imgui_impl_glfw.h"
#include "imgui_docking/imgui_impl_opengl3.h"

#include "../../engine/asset_repository.h"

class AssetWindow {
private:
	AssetRepository* assetRepository;
public:
	bool open = true;

	AssetWindow(AssetRepository* assetRepository) {
		this->assetRepository = assetRepository;
	}


	void draw() {
		if (open) {
			ImGui::Begin("Assets", &open);
			if (ImGui::TreeNodeEx("Textures")) {
				for (auto const& pair : assetRepository->texturesMap) {
					Texture* texture = pair.second;
					ImGui::Text("assID: %d", texture->assId);
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Text(texture->name.c_str());
					ImGui::PushID(texture->assId);
					ImGui::ImageButton("Texture", texture->glid, ImVec2(64, 64));
					if (ImGui::BeginDragDropSource()) {
						ImGui::SetDragDropPayload("TEXTURE", &texture->assId, sizeof(texture->assId));
						ImGui::Text(texture->name.c_str());
						ImGui::EndDragDropSource();
					}
					ImGui::PopID();
				}

				ImGui::TreePop();
			}
			if (ImGui::TreeNodeEx("Vertices")) {
				for (auto const& pair : assetRepository->verticesMap) {
					Vertices* vertices = pair.second;
					ImGui::Text("assID: %d", vertices->assId);
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Text(vertices->name.c_str());
					ImGui::SameLine();
					ImGui::PushID(vertices->assId);
					ImGui::Button("Drag");
					if (ImGui::BeginDragDropSource()) {
						ImGui::SetDragDropPayload("VERTICES", &vertices->assId, sizeof(vertices->assId));
						ImGui::Text(vertices->name.c_str());
						ImGui::EndDragDropSource();
					}
					ImGui::PopID();
				}

				ImGui::TreePop();
			}
			if (ImGui::TreeNodeEx("Materials")) {
				for (auto const& pair : assetRepository->materialsMap) {
					Material* material = pair.second;
					ImGui::Text("assID: %d", material->assId);
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Text(material->name.c_str());
					ImGui::SameLine();
					ImGui::PushID(material->assId);
					ImGui::Button("Drag");
					if (ImGui::BeginDragDropSource()) {
						ImGui::SetDragDropPayload("MATERIAL", &material->assId, sizeof(material->assId));
						ImGui::Text(material->name.c_str());
						ImGui::EndDragDropSource();
					}
					ImGui::PopID();
				}

				ImGui::TreePop();
			}


			ImGui::End();
		}
	}
};