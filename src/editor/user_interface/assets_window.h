#pragma once

#include <string>
#include <filesystem>
#include <typeinfo>

#include "imgui_docking/imgui.h"
#include "imgui_docking/imgui_impl_glfw.h"
#include "imgui_docking/imgui_impl_opengl3.h"

#include "../../engine/asset_repository.h"
#include "../../engine/loaders/image_loader.h"

class AssetWindow
{
private:
	AssetRepository *assetRepository;

public:
	bool open = true;

	std::filesystem::path *drishPath = nullptr;

	AssetWindow(AssetRepository *assetRepository)
	{
		this->assetRepository = assetRepository;
	}

	template <typename T>
	void drawList(std::string name, std::string payloadName, std::vector<AssetSlot<T>*> slots,
		std::function<void(int)> deleteFunction, std::function<void()> createFunction) {
		if (ImGui::TreeNodeEx(name.c_str()))
		{
			if (ImGui::Button("+"))
			{
				createFunction();
			}
			for (const AssetSlot<T>* assetSlot : slots)
			{
				ImGui::PushID(assetSlot->index);
				if (assetSlot->is_valid && ImGui::TreeNodeEx(assetSlot->asset->name.c_str(), ImGuiTreeNodeFlags_Leaf)) {
					T* asset = assetSlot->asset;

					if (ImGui::BeginPopupContextItem())
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
						if (ImGui::MenuItem("Delete")) {
							deleteFunction(assetSlot->index);
						}
						ImGui::PopStyleColor();
						ImGui::EndPopup();
					}

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload(payloadName.c_str(), &assetSlot->index, sizeof(assetSlot->index));
						ImGui::Text(asset->name.c_str());
						ImGui::EndDragDropSource();
					}

					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}

	void draw()
	{
		if (open)
		{
			ImGui::Begin("Assets", &open);
			//if (ImGui::TreeNodeEx("Textures"))
			//{
			//	for (auto const &pair : assetRepository->texturesMap)
			//	{
			//		std::shared_ptr<Texture> texture = pair.second;
			//		ImGui::Text("assID: %d", texture->assId);
			//		ImGui::SameLine();
			//		ImGui::Spacing();
			//		ImGui::SameLine();
			//		ImGui::Text(texture->name.c_str());
			//		ImGui::PushID(texture->assId);
			//		ImGui::ImageButton("Texture", texture->glid, ImVec2(64, 64));
			//		if (ImGui::BeginDragDropSource())
			//		{
			//			ImGui::SetDragDropPayload("TEXTURE", &texture->assId, sizeof(texture->assId));
			//			ImGui::Text(texture->name.c_str());
			//			ImGui::EndDragDropSource();
			//		}
			//		ImGui::PopID();
			//	}

			//	if (ImGui::Button("+", ImVec2(64, 64)))
			//	{
			//		/*std::filesystem::path texturePath = drishengine::openImageOpenFileDialog();
			//		if (!texturePath.empty())
			//		{
			//			std::filesystem::path filename = texturePath.filename();
			//			Texture* texture = new Texture(-1);
			//			texture->name = filename.string();
			//			ImageLoader::loadImage(texturePath, texture);
			//			assetRepository->addTexture(texture);
			//		}*/
			//	}

			//	ImGui::TreePop();
			//}
			drawList<Vertices>(
				"Vertices",
				"VERTICES",
				assetRepository->vertices.slots,
				[this](int index) {
					assetRepository->vertices.remove(index);
				},
				[this]() {
					std::filesystem::path objFilePath = drishengine::openModelOpenFileDialog();

					if (!objFilePath.empty())
					{
						std::filesystem::path filename = objFilePath.filename();
						filename.replace_extension("model");

						Vertices* vertices = new Vertices();
						vertices->name = objFilePath.stem().string();

						vertices->path = std::filesystem::path("models") / filename;
						drishengine::loadObj(objFilePath, vertices);

						assetRepository->vertices.add(vertices);

						if (drishPath != nullptr)
						{
							if (std::filesystem::exists(drishPath->parent_path() / "models"))
							{
								logDebug("[ASSETS WINDOW] models exists");
							}
							else
							{
								std::filesystem::create_directory(drishPath->parent_path() / "models");
							}

							drishengine::writeModelData(drishPath->parent_path() / "models" / filename,
								vertices->data);
							vertices->createBuffers();
						}
						else
						{
							logError("[ASSETS WINDOW] drishPath is null");
						}
						}
					}
				);


			drawList<Material>(
				"Materials",
				"MATERIAL",
				assetRepository->materials.slots,
				[this](int index) {
					assetRepository->materials.remove(index);
				},
				[this]() {
					Material* mat = new Material();
					mat->name = "New material";
					mat->shader = &assetRepository->defaultShader;
					assetRepository->materials.add(mat);
				}
			);

	
			ImGui::End();
		}
	}
};